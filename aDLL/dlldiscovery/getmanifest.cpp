#include "adll.h"

BOOL GetManifest(ARGS Args, LPWSTR* pManifest)
{
    HMODULE hExe = LoadLibraryEx(Args.EPath, NULL, LOAD_LIBRARY_AS_DATAFILE);
    if (!hExe)
    {
        PrintError("GetManifest - GetmoduleHandle()");
        return FALSE;
    }
    HRSRC ResInfo = FindResourceW(hExe, MAKEINTRESOURCEW(1), RT_MANIFEST);
    if (!ResInfo)
    {
        PrintError("GetManifest - FindResource()");
        FreeLibrary(hExe);
        return FALSE;
    }
    HGLOBAL ResData = LoadResource(hExe, ResInfo);
    if (!ResData)
    {
        PrintError("GetManifest - LoadResource()");
        FreeLibrary(hExe);
        return FALSE;
    }
    DWORD ResSize = SizeofResource(hExe, ResInfo);
    if (!ResSize)
    {
        PrintError("GetManifest - SizeofResource()");
        FreeLibrary(hExe);
        return FALSE;
    }
    LPVOID Manifest = LockResource(ResData);
    if (!Manifest)
    {
        PrintError("GetManifest - LockResource()");
        FreeLibrary(hExe);
        return FALSE;
    }
    *pManifest = AnsiToWide((char*)Manifest);
    FreeLibrary(hExe);
    return TRUE;
}