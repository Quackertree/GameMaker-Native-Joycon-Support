#include "stdafx.h"
#include "hidapi.h"

#include <string>
#include <iostream>
#include <chrono>
#include <thread>
#include <atomic>

#define GMEXPORT extern "C" __declspec(dllexport)
#define DATA_BUFFER_SIZE 40
#define ANGLE_PRECISION (1.0f / 1000.0f);
#define SPEED_PRECISION -(1.8f / 1023.0f / 7.4f);

//Vars
const unsigned short NINTENDO = 1406; //0x057e
const unsigned short JOYCON_L = 8198; //0x2006
const unsigned short JOYCON_R = 8199; //0x2007

const unsigned short LEFT = 0;
const unsigned short RIGHT = 1;
hid_device *joycon[] = { NULL, NULL };

unsigned char joyDataStreamLeft[DATA_BUFFER_SIZE];
unsigned char joyDataStreamRight[DATA_BUFFER_SIZE];
unsigned char *dataStream[] = { joyDataStreamLeft, joyDataStreamRight };

int buttonStates[] = { 0, 0 };
int buttonStatesLast[] = { 0, 0 };

int lastAcceleration[2][3] = { { 0, 0, 0 }, { 0, 0, 0 } };

std::atomic<bool> running{ true };
std::thread* thread;

//"Lazy declare" functions (= in call order, no header file)
//All functions are double because otherwise GM crashes sometimes? Perhaps it was something else that caused it, though

//Functions
void updateButtons(short device)
{
	if (device == 0)
		buttonStates[device] = dataStream[device][5] + (dataStream[device][4] << 8);
	else
		buttonStates[device] = dataStream[device][3] + (dataStream[device][4] << 8);
}

void read(short device)
{
	hid_read_timeout(joycon[device], dataStream[device], DATA_BUFFER_SIZE, 20);
	updateButtons(device);
}

void loop(std::atomic<bool>& isRunning)
{
	while (isRunning)
	{
		bool noLeft = false, noRight = false;
		std::this_thread::sleep_for(std::chrono::milliseconds(2));

		try {
			if (joycon[RIGHT] != NULL)
				read(RIGHT);
		}
		catch (std::exception e) {
			noRight = true;
		}

		try {
			if (joycon[LEFT] != NULL)
				read(LEFT);
		}
		catch (std::exception e) {
			noLeft = true;
		}

		if (noLeft || noRight)
			isRunning = false;
	}
}

//Threading stuff
GMEXPORT double init()
{
	memset(joyDataStreamLeft, 0, DATA_BUFFER_SIZE);
	memset(joyDataStreamRight, 0, DATA_BUFFER_SIZE);

	thread = new std::thread(loop, std::ref(running));

	return 1.0f;
}

GMEXPORT double stop()
{
	try {
		running = false;
		thread->join();
	}
	catch (std::exception e) {}

	hid_exit();

	return 1.0f;
}

//Ext
float clamp(float x, float upper, float lower) {
	if (x > upper)
		return upper;
	else if (x < lower)
		return lower;
	else
		return x;
}

int16_t uint16_to_int16(uint16_t a) {
	int16_t b;
	char* aPointer = (char*)&a, *bPointer = (char*)&b;
	memcpy(bPointer, aPointer, sizeof(a));
	return b;
}

inline float radtodeg(float rad) {
	return (rad * (180.0f) / (2.0f * 3.1415926f));
}

//Data pull functions
GMEXPORT double getAccelerationZRaw(double device) {
	return (double)uint16_to_int16(dataStream[(short)device][13] | (dataStream[(short)device][14] << 8) & 0xFF00) * SPEED_PRECISION;
}

GMEXPORT double getAccelerationXRaw(double device) {
	return (double)uint16_to_int16(dataStream[(short)device][15] | (dataStream[(short)device][16] << 8) & 0xFF00) * SPEED_PRECISION;
}

GMEXPORT double getAccelerationYRaw(double device) {
	return (double)uint16_to_int16(dataStream[(short)device][17] | (dataStream[(short)device][18] << 8) & 0xFF00) * SPEED_PRECISION;
}

GMEXPORT double getAccelerationX(double device) {
	return (double)(lastAcceleration[(short)device][0] - getAccelerationXRaw(device));
}

GMEXPORT double getAccelerationY(double device) {
	return (double)(lastAcceleration[(short)device][1] - getAccelerationYRaw(device));
}

GMEXPORT double getAccelerationZ(double device) {
	return (double)(lastAcceleration[(short)device][2] - getAccelerationZRaw(device));
}

GMEXPORT double getRotationZ(double device) {
	double rot = -(double)uint16_to_int16(dataStream[(short)device][19] | (dataStream[(short)device][20] << 8) & 0xFF00) * ANGLE_PRECISION;
	return (abs(rot) >= 0.1f) ? rot : 0.0f;
}

GMEXPORT double getRotationX(double device) {
	double rot = (double)uint16_to_int16(dataStream[(short)device][21] | (dataStream[(short)device][22] << 8) & 0xFF00) * ANGLE_PRECISION;
	return (abs(rot) >= 0.1f) ? rot : 0.0f;
}

GMEXPORT double getRotationY(double device) {
	double rot = -(double)uint16_to_int16(dataStream[(short)device][23] | (dataStream[(short)device][24] << 8) & 0xFF00) * ANGLE_PRECISION;
	return (abs(rot) >= 0.1f) ? rot : 0.0f;
}

GMEXPORT double getAnalogRaw(double device, double axis)
{
	if(joycon[(short)device] == NULL)
		return 0.0f;
	
	float val;
	if ((short)device == 1)
	{
		if (axis == 0) { //Horizontal
			val = clamp((dataStream[(short)device][9] | ((dataStream[(short)device][10] & 0xF) << 8)) / 1000.0f - 2.273f, 1, -1);
		}
		else { //Vertical
			val = -clamp((((dataStream[(short)device][10] >> 4) | (dataStream[(short)device][11] << 4)) - 1778.0f) / 1100.0f, 1, -1);
		}
	}
	else
	{
		if (axis == 0) { //Horizontal
			val = clamp(((dataStream[(short)device][6] | ((dataStream[(short)device][7] & 0xF) << 8)) - 1900.0f) / 1300.0f, 1, -1);
		}
		else { //Vertical
			val = -clamp((((dataStream[(short)device][7] >> 4) | (dataStream[(short)device][8] << 4)) - 2340.0f) / 1000.0f, 1, -1);
		}
	}

	return val;
}

GMEXPORT double getAnalog(double device, double axis)
{
	float val = getAnalogRaw(device, axis);
	return (abs(val) >= 0.15f) ? val : 0.0f;
}

GMEXPORT double getRawByte(double device, double index)
{
	return (dataStream[(short)device][(int)index]);
}

GMEXPORT double getButton(double device, double buttonID)
{
	return (buttonStates[(short)device] & (int)buttonID) > 0;
}

GMEXPORT double getButtonPressed(double device, double buttonID)
{
	return ((buttonStates[(short)device] & (int)buttonID) > 0 && (buttonStatesLast[(short)device] & (int)buttonID) <= 0);
}

GMEXPORT double getButtonReleased(double device, double buttonID)
{
	return ((buttonStates[(short)device] & (int)buttonID) <= 0 && (buttonStatesLast[(short)device] & (int)buttonID) > 0);
}

GMEXPORT double handleButtonInput()
{
	for (int i = 0; i < 2; i++)
	{
		buttonStatesLast[i] = buttonStates[i];

		lastAcceleration[i][0] = -getAccelerationXRaw(i);
		lastAcceleration[i][1] = -getAccelerationYRaw(i);
		lastAcceleration[i][2] = -getAccelerationZRaw(i);
	}

	return 1.0f;
}

//Connection
void sendInitPackage(short device, int packetValue, int packetSupport)
{
	unsigned char buf[DATA_BUFFER_SIZE];
	memset(buf, 0, DATA_BUFFER_SIZE);

	buf[0] = 0x1;
	buf[10] = packetSupport;
	buf[11] = packetValue;

	int res = hid_write(joycon[device], buf, 12);
	res = hid_read(joycon[device], buf, DATA_BUFFER_SIZE);
}

//Data set functions
GMEXPORT double setLight(double device, double bitflag) {
	if(joycon[(short)device] != NULL)
	{
		sendInitPackage((short)device, (int)bitflag, 48);
		return 1.0f;
	}
	else
		return 0.0f;
}

GMEXPORT double connect(double device)
{
	unsigned short s;

	//Select joycon code
	if (device == LEFT)
		s = JOYCON_L;
	else if (device == RIGHT)
		s = JOYCON_R;
	else
		return false;

	//Detect joycon on network
	struct hid_device_info *joycon_info = hid_enumerate(NINTENDO, s);
	if (joycon_info == NULL)
		return 0.0f;

	//Open joycon for reading
	joycon[(short)device] = hid_open(NINTENDO, s, joycon_info->serial_number);

	//Init joycon report modes
	sendInitPackage(device, 0x30, 0x03); //Auto refresh @60 Hz
	sendInitPackage(device, 0x01, 0x40); //Enable motion data pulling
	setLight(device, 1);

	//Disable blocking
	hid_set_nonblocking(joycon[(short)device], true);

	return 1.0f;
}