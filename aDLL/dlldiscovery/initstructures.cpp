#include "adll.h"

DLLINFO* InitDLLInfo(size_t size)
{
	DLLINFO* DLL = NULL;
	size_t i = 0;

	DLL = (DLLINFO*)calloc(size, sizeof(DLLINFO));
	if (DLL == NULL)
	{
		printf("[!] ERROR: InitDLLInfo - calloc couldn't allocate memory\n");
		return NULL;
	}
	for (i = 0; i < size; i++)
	{
		DLL[i].Name = NULL;
		DLL[i].Import = L"";
		DLL[i].dFlag = 0;
		DLL[i].pFlag = L"";
		DLL[i].Known = 0;
		DLL[i].Found = 0;
		DLL[i].FPath = L"";
		DLL[i].ReLvl = 0;
		DLL[i].ReDLLs = NULL;
	}
	return DLL;
}

VOID InitSchPaths(SCHPATHS *SPaths, LPCWSTR fPath, WORD Arch)
{
    WCHAR	Buff[MAX_PATH] = { 0 };
    size_t  Len = 0;
    // Get the executable directory
	Len = wcslen(fPath);
	while (fPath[Len] != '\\')
	{
		Len--;
	}
	wcsncpy_s(Buff, _countof(Buff), fPath, Len);
	SPaths->AppDir = _wcsdup(Buff);

    ZeroMemory(Buff, MAX_PATH);
    // 32-bit applications are redirected by the File System to SysWoW64 directory if they try to acces to System32
    if (Arch == 32)
        GetSystemWow64Directory(Buff, MAX_PATH);
    else
        GetSystemDirectoryW(Buff, MAX_PATH);
    SPaths->SysDir32 = _wcsdup(Buff);

	// There is no function that retrieves the path to the System (16-bit) directory
	if (PathIsDirectoryW(L"C:\\Windows\\System"))
		SPaths->SysDir16 = _wcsdup(L"C:\\Windows\\System");
	else
		SPaths->SysDir16 = NULL;

    ZeroMemory(Buff, MAX_PATH);
	GetWindowsDirectoryW(Buff, MAX_PATH);
	SPaths->WinDir = _wcsdup(Buff);

    ZeroMemory(Buff, MAX_PATH);
	GetCurrentDirectoryW(MAX_PATH, Buff);
	SPaths->CWDir = _wcsdup(Buff);

    
    WCHAR	PATHBuff[PATHENVMAX] = { 0 };
	GetEnvironmentVariableW(L"PATH", PATHBuff, PATHENVMAX);
	SPaths->PATHDirs = _wcsdup(PATHBuff);
	return;
}

