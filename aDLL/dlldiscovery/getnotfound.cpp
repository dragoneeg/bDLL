#include "adll.h"

/*
* GetNotFound is the function that search the locations of a DLL in disk
* in order to get a list of those DLLs imported by the binary that are not 
* found. That may lead to some of those DLLs being vulnerable to hijacking.
*/

// This function is called every time a DLL is searched in a given path
BOOL TryPath(DLLINFO* DLL, LPCWSTR SPath, LPWSTR NotFoundBuff)
{
    WCHAR   TestFilePath[1023] = { 0 };

    wcscpy_s(TestFilePath, _countof(TestFilePath), SPath);
    // If the path doesn't have a backslash at the end append it
    size_t len = wcslen(TestFilePath);
    if (TestFilePath[len - 1] != L'\\')
        wcscat_s(TestFilePath, _countof(TestFilePath), L"\\");
    // Append the DLL Name to the Path
    wcscat_s(TestFilePath, _countof(TestFilePath), DLL->Name);
    if (!PathFileExistsW(TestFilePath))
    {
        //printf("- %ws NOT FOUND in %ws\n", DLL->Name, TestFilePath);
        wcscat_s(NotFoundBuff, BUFFSIZE, L"- ");
        wcscat_s(NotFoundBuff, BUFFSIZE, DLL->Name);
        wcscat_s(NotFoundBuff, BUFFSIZE, L" NOT FOUND in ");
        wcscat_s(NotFoundBuff, BUFFSIZE, TestFilePath);
        wcscat_s(NotFoundBuff, BUFFSIZE, L"\n");
        DLL->Found = 0;
        if (DLL->FPath)
            //free((PVOID)DLL->FPath);
        DLL->FPath = L"";
        return FALSE;
    }
    else 
    {
        DLL->Found = 1;
        if (DLL->FPath != NULL)
            //free((PVOID)DLL->FPath);
        DLL->FPath = _wcsdup(TestFilePath);
        return TRUE;
    }
}

VOID GetNotFound(DLLINFO* DLL, SCHPATHS SPaths, INT DLLNum, LPWSTR NotFoundBuff)
{
    INT     i;

    i = 0;
    while (i < DLLNum)
    {
        // COMCTL32 is always redirected by the side by side assembly system
        if ((wcsstr(DLL[i].Name, L"COMCTL32") != NULL) || (wcsstr(DLL[i].Name, L"comctl32") != NULL))
        {
            i++;
            continue;
        }
        // Known DLLs are already loaded and mapped into the process memory without searching them in disk
        if (DLL[i].Known == 1)
        {
            DLL[i].Found = 1;
            DLL[i].FPath = L"cached in memory";
            i++;
            continue;
        }
        // DLLs whose names start by "api-ms-" or "ext-ms-" are virtual DLLs and are redirected to other DLLs
        if (wcsstr(DLL[i].Name, L"api-ms") != NULL || wcsstr(DLL[i].Name, L"ext-ms") != NULL)
        {
            WCHAR  RealDLL[MAX_PATH] = { 0 };
            HMODULE hModule = GetModuleHandleW(DLL[i].Name);
            GetModuleFileNameW(hModule, RealDLL, MAX_PATH);
            if (PathFileExistsW(RealDLL))
            {
                DLL[i].FPath = _wcsdup(RealDLL);
                DLL[i].Found = 1;
            }
            i++;
            continue;
        }
        // DLL has been loaded by a LoadLibraryEx function given with a FLAG
        if (DLL[i].dFlag != 0)
        {
            if (DLL[i].dFlag == 8)
            {
                WCHAR   Dir[_MAX_DIR] = { 0 };
                WCHAR   FName[_MAX_FNAME] = { 0 };
                WCHAR   Ext[_MAX_EXT] = { 0 };

                _wsplitpath_s(DLL[i].Name, NULL, 0, Dir, MAX_PATH, FName, _MAX_FNAME, Ext, _MAX_EXT);
                wcscat_s(FName, _MAX_FNAME, Ext);
                DLL[i].Name = _wcsdup(FName);
                // Hold the path of the DLL. Its dependencies will be searched in that path.
                DLL[i].IPath = _wcsdup(Dir);
                if (TryPath(&DLL[i], Dir, NotFoundBuff))
                {
                    i++;
                    continue;
                }
                if (TryPath(&DLL[i], SPaths.SysDir32, NotFoundBuff))
                {
                    i++;
                    continue;
                }
                if (TryPath(&DLL[i], SPaths.SysDir16, NotFoundBuff))
                {
                    i++;
                    continue;
                }
                if (TryPath(&DLL[i], SPaths.WinDir, NotFoundBuff))
                {
                    i++;
                    continue;
                }
            }
            else if (DLL[i].dFlag == 512)
                TryPath(&DLL[i], SPaths.AppDir, NotFoundBuff);
            else if (DLL[i].dFlag == 2048 || DLL[i].dFlag == 16384)
                TryPath(&DLL[i], SPaths.SysDir32, NotFoundBuff);
            else if (DLL[i].dFlag == 4096 || DLL[i].dFlag == 4352)
                if (!TryPath(&DLL[i], SPaths.AppDir, NotFoundBuff))
                    TryPath(&DLL[i], SPaths.SysDir32, NotFoundBuff);
        }
        // When DLL is passed to LoadLibrary() as a full path is only searched in that Path
        else if ((wcschr(DLL[i].Name, '\\') != NULL))
        {
            WCHAR   Dir[_MAX_DIR] = { 0 };
            WCHAR   FName[_MAX_FNAME] = { 0 };
            WCHAR   Ext[_MAX_EXT] = { 0 }; 

            _wsplitpath_s(DLL[i].Name, NULL, 0, Dir, _MAX_DIR, FName, _MAX_FNAME, Ext, _MAX_EXT);
            wcscat_s(FName, _MAX_FNAME, Ext);
            DLL[i].Name = _wcsdup(FName);
            TryPath(&DLL[i], Dir, NotFoundBuff);
            /*
            if (!PathFileExistsW(DLL[i].Name))
            {
                //printf("- %ws NOT FOUND\n", DLL[i].Name);
                wcscat_s(NotFoundBuff, BUFFSIZE, DLL[i].Name);
                wcscat_s(NotFoundBuff, BUFFSIZE, L" NOT FOUND\n");
                DLL->Found = 0;
            }*/
        }
        // This is the case when the DLL is searched with the regular search order
        else
        {
            if (TryPath(&DLL[i], SPaths.AppDir, NotFoundBuff))
            {
                i++;
                continue;
            }
            if (TryPath(&DLL[i], SPaths.SysDir32, NotFoundBuff))
            {
                i++;
                continue;
            }
            if (TryPath(&DLL[i], SPaths.SysDir16, NotFoundBuff))
            {
                i++;
                continue;
            }
            if (TryPath(&DLL[i], SPaths.WinDir, NotFoundBuff))
            {
                i++;
                continue;
            }
            if (TryPath(&DLL[i], SPaths.CWDir, NotFoundBuff))
            {
                i++;
                continue;
            }
            // Search DLL in PATH enviroment variable directories
            LPWSTR  Token = NULL;
            LPWSTR  NextToken = NULL;
            WCHAR   PATHD[1024] = { 0 };
            size_t  len = 0;

            // Split PATH variable value into directories strings
            wcscpy_s(PATHD, _countof(PATHD), SPaths.PATHDirs);
            Token = wcstok_s(PATHD, L";", &NextToken);
            while (Token)
            {
                // Remove final backslash for those directories retrieved from %PATH% that have it
                len = wcslen(Token);
                if (Token[len - 1] == L'\\')
                    Token[len - 1] = L'\0';
                // Search DLL in the splitted directory
                if (TryPath(&DLL[i], Token, NotFoundBuff))
                    break;
                Token = wcstok_s(NULL, L";", &NextToken);
            }
        }
        i++;
    }
    return ;
}
