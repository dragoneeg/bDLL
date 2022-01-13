#include "dlldiscovery.h"

VOID InitDLLInfo(DLLINFO* DLL, SIZE_T count)
{
	SIZE_T i;

	for (i = 0; i < count; i++)
	{
		DLL[i].Name = NULL;
		DLL[i].Import = L"";
		DLL[i].Flag = 0;
		DLL[i].Known = 0;
		DLL[i].Found = 0;
		DLL[i].FPath = L"";
		DLL[i].ReLvl = 0;
	}
	return;
}

VOID InitSchPaths(SCHPATHS *SPaths, LPCWSTR fPath)
{
	WCHAR	Buff[1023] = { 0 };
	SIZE_T	Len;

	Len = wcslen(fPath);
	while (fPath[Len] != '\\')
	{
		Len--;
	}
	wcsncpy_s(Buff, _countof(Buff), fPath, Len);
	SPaths->AppDir = _wcsdup(Buff);
	SecureZeroMemory(Buff, 1023);

	GetSystemDirectory(Buff, 1023);
	SPaths->SysDir32 = _wcsdup(Buff);
	SecureZeroMemory(Buff, 1023);

	// There is no function that retrieves the path to the System (16-bit) directory
	// Check if the Path is valid for the current system
	SPaths->SysDir16 = _wcsdup(L"C:\\Windows\\System");

	GetWindowsDirectory(Buff, 1023);
	SPaths->WinDir = _wcsdup(Buff);
	SecureZeroMemory(Buff, 1023);

	GetCurrentDirectory(1023, Buff);
	SPaths->CWDir = _wcsdup(Buff);
	SecureZeroMemory(Buff, 1023);

	GetEnvironmentVariableW(L"PATH", Buff, 1023);
	SPaths->PATHDirs = _wcsdup(Buff);
	SecureZeroMemory(Buff, 1023);
	return;
}

VOID InitStructures(DLLINFO *DLL, SIZE_T Count, SCHPATHS *SPaths, LPCWSTR fPath)
{
	InitDLLInfo(DLL, Count);
	InitSchPaths(SPaths, fPath);
	return;
}

