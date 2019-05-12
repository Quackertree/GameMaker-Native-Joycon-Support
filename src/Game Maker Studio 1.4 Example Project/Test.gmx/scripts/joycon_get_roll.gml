///joycon_get_roll(<int> device)
/*
    Returns the roll (as float) of the joycon between -90 and 90.
    The roll flips around if held upside down (for some reason?)
*/

return external_call(external_define("./BluetoothDLLGMJoy.dll", "getRoll", dll_cdecl, ty_real, 1, ty_real), argument0);
