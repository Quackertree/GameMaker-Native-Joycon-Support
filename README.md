# GameMaker-Native-Joycon-Support

This is an extension for Game Maker Studio 1.4, including source, which allows you to pair your Joycon to your Windows machine and read the input from it.

Note: This is made for homebrews who want to toy with the Joycon. Therefore, the input received from the Joycon may not be perfect. If you seek perfect synchronisation, get an actual Nintendo Dev Account.

Features:
- Support for both the left and right Joycon, either individually or paired.
- Get button input in the form of *_check, *_pressed and *_released, just like you know and love in native GM:S
- Read the analog stick in horizontal and vertical direction, with integrated dead center check
- Retrieve relative angular velocity input, allowing you to detect rotation of your Joycon
- Retrieve absolute acceleration to detect shaking or generic movement of each Joycon
- Control each of the four LEDs on the Joycon individually
- Additional raw data pull functions for more precise measurements or wanted extended functionality

Please be aware that the Joycons do NOT individually keep track of their pitch, roll and yaw by default, which means you cannot perform tracking like e.g. a virtual reality controller. Although theoretically extractable information from the accelerometer and gyroscope paired together, this extension does not support such features!

It is highly recommended to look at the example project in order to understand the functions.

Be aware some functions may seem to return unexpected values, but this is just the way the Joycon operates. More information on this subject can be found in the example project.

If you find any significant bugs, please let me know!

Credit to: https://github.com/mfosse/JoyCon-Driver/blob/master/joycon-driver/src/main.cpp for figuring out a lot of stuff, allowing me to put it together in a DLL for GM:S.
