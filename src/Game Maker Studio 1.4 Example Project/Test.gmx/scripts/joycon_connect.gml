///joycon_connect(<int> device)
/*
    Will detect the ALREADY PAIRED joycon and store the pointer to this joycon for further joycon_get_* calls.
    The pointer is stored internally by the DLL, so you won't need to worry about it.
    
    Returns a bool indicating connection succes.
*/

show_debug_message("Connecting the joycon...");
return external_call(external_define("BluetoothDLLGMJoy.dll", "connect", dll_cdecl, ty_real, 1, ty_real), argument0);
