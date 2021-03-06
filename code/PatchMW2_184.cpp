// ==========================================================
// MW2 coop
// 
// Component: IW4SP
// Sub-component: clientdll
// Purpose: Patches for version 184
//
// Initial author: momo5502
// Started: 2014-03-05
// ==========================================================

#include "StdInc.h"
#include "184_defs.h"

void connectWrapper();
void PatchMW2_Branding();
void PatchMW2_NoBorder();
void PatchMW2_Minidump();
void PatchMW2_Images();
void PatchMW2_LocalizedStrings();
void PatchMW2_Load();
void PatchMW2_UILoading();
void PatchMW2_Script();
void PatchMW2_Steam();
void PatchMW2_MusicalTalent();
void PatchMW2_ConsoleStart();
void PatchMW2_Weapons();
void PatchMW2_AssetRestrict();
void PatchMW2_Achievement();
void PatchMW2_Icon();
void PatchMW2_Materialism();
void PatchMW2_RecoverDevice();
void PatchMW2_Coop();
void PatchMW2_OOB();
void PatchMW2_ClientConsole();

dvar_t* nameHookFunc184(const char* name, const char* defaultVal, int flags, const char* description)
{
	Dvar_RegisterString("connect_ip", va("127.0.0.1:%d", *(DWORD*)0x48C5F7), DVAR_FLAG_SAVED, "Temporary dvar used to connect to coop.");

	return Dvar_RegisterString(name, defaultVal, DVAR_FLAG_SAVED | DVAR_FLAG_SERVER | DVAR_FLAG_USERINFO, description);
}

DWORD SteamUserStuff184 = 0x4D0710;
DWORD returnSuccess184 = 0x47BE79;
DWORD otherStuff = 0x4257D0;

void __declspec(naked) steamInitPatch184()
{
	__asm
	{
		call SteamUserStuff184
		test al, al
		jz returnSafe
		jmp returnSuccess

returnSuccess:
		call otherStuff
		test al, al
		jz returnSafe
		jmp returnSuccess184

returnSafe:
		mov al, 1
		retn
	}
}

void xstopparty_f();
void patchExit(DWORD address);

void PatchMW2_184()
{
	define184Stuff();

	PatchMW2_Minidump();
	PatchMW2_Branding();
	PatchMW2_NoBorder();
	PatchMW2_Images();
	PatchMW2_LocalizedStrings();
	PatchMW2_Load();
	PatchMW2_UILoading();
	PatchMW2_Script();
	PatchMW2_Steam();
	PatchMW2_MusicalTalent();
	PatchMW2_ConsoleStart();
	PatchMW2_Weapons();
	PatchMW2_AssetRestrict();
	PatchMW2_Achievement();
	PatchMW2_Icon();
	PatchMW2_Materialism();
	PatchMW2_RecoverDevice();
	PatchMW2_Coop();
	PatchMW2_OOB();
	PatchMW2_ClientConsole();

	// Force external console
	memset((void*)0x60182F, 0x90, 23);

	// Ignore protocol mismatch 
	*(BYTE*)0x65A952 = 0xEB;
	*(BYTE*)0x65A97D = 0xEB;

	// Disable Matchmaking stuff
	*(BYTE*)0x4DAA10 = 0xC3;

	// Ignore XUID match
	*(BYTE*)0x65AA3F = 0xEB;

	// Ignore 'MAX_PACKET_USERCMDS'
	*(BYTE*)0x4C6FA6 = 0xEB;

	// Fake login
	*(BYTE*)0x4F1F53 = 0xEB;
	*(BYTE*)0x57A5C3 = 0xEB;
	*(BYTE*)0x57A890 = 0xEB;

	// Disable IWNet stuff
	*(BYTE*)0x4165B4 = 0xEB;

	// Change 'connect' to 'connect_coop'
	*(DWORD*)0x4F3B27 = (DWORD)"connect_coop";

	// version string
	*(DWORD*)0x601A2F = (DWORD)(CONSOLESTRING);

	// Remove ''
	nop(0x600776, 5);

	// Apply m2demo stuff
	*(DWORD*)0x62ED01 = (DWORD)BASEGAME;

	// Change window titles
	*(DWORD*)0x414828 = (DWORD)"COD-MOD: Console";
	*(DWORD*)0x50B880 = (DWORD)"COD-MOD: Game";

	// Flag cg_fov as saved
	*(BYTE*)0x47C165 = DVAR_FLAG_SAVED;

	// Show intro (or not)
	*(BYTE*)0x600D6D = 0;

	// Unflag dvar intro
	*(BYTE*)0x600D6B = 0;

	// Ignore config problems
	*(BYTE*)0x496AA3 = 0xEB;

	// Video folders
	*(DWORD*)0x509782 = 0x7204A0; // raw -> main
	*(DWORD*)0x509764 = (DWORD)"%s\\" BASEGAME "\\video\\%s.bik"; // main -> data

	// Ignore 'steam must be running' error
	nop(0x601863, 0x30);

	// Ignore iw.net resolve errors
	nop(0x40868F, 5);

	// Patch steam auth
	nop(0x47BE55, 7);
	*(BYTE*)0x47BE5C = 0xEB;
	call(0x47BE70, steamInitPatch184, PATCH_JUMP);

	// Force debug logging
	nop(0x4B3AE5, 2);

	// Remove dvar restrictions
	*(BYTE*)0x633071 = 0xEB; // read only
	*(BYTE*)0x633143 = 0xEB; // cheat protected
	*(BYTE*)0x6330D5 = 0xEB; // write protected
	*(BYTE*)0x6331A4 = 0xEB; // latched

	// No improper quit popup
	memset((void*)0x41796A, 0x90, 2);

	// Yay, hitmarker in sp :D
	Dvar_RegisterBool("scr_damageFeedback", 0, DVAR_FLAG_SAVED, "Show marker when hitting enemies.");

	static cmd_function_t connectWrapper_cmd;
	Cmd_AddCommand("connect", connectWrapper, &connectWrapper_cmd, 0);

	static cmd_function_t xstopparty_cmd;
	Cmd_AddCommand("xstopparty", xstopparty_f, &xstopparty_cmd, 0);

	// Patch dvar name
	call(0x4F3866, nameHookFunc184, PATCH_CALL);

	// Ignore savegame checksum mismatch
	//nop(0x4BFBC9, 5);

	// CEG again?
	patchExit(0x424CB5);
	patchExit(0x474E15);
	patchExit(0x410875);
	patchExit(0x401195);
	patchExit(0x4516F5);
	patchExit(0x427545);
	patchExit(0x4A9F85);
	patchExit(0x478985);
	patchExit(0x4A6745);
	patchExit(0x4093E5);
}