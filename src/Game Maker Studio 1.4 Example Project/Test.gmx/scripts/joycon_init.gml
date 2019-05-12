///joycon_init()
/*
    Call this function at game start to enable start the data collection thread.
    Likewise, call the joycon_exit() function when the game ends to make sure the thread closes again (otherwise it will remain running in the background! O_O)
*/

enum JoyconDevice {
    LEFT,
    RIGHT
};

external_call(external_define("./BluetoothDLLGMJoy.dll", "init", dll_cdecl, ty_real, 0));
