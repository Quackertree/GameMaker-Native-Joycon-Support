///joycon_exit()
/*
    Ends the data collection thread safely. Must be called on game end.
*/

external_call(external_define("./BluetoothDLLGMJoy.dll", "stop", dll_cdecl, ty_real, 0));
