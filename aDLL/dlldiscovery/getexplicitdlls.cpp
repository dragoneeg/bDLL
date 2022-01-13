#include "adll.h"
#include "..\detours\detours.h"

#ifdef _WIN32
#pragma comment(lib, "..\\detours\\lib.X86\\detours.lib")
#endif

#ifdef _WIN64
#pragma comment(lib, "..\\detours\\lib.X64\\detours.lib")
#endif

BOOL HookInjector(LPWSTR pExePath, WORD Arch, DWORD seconds)
{
    // Strutures needed to create a proces:
    PROCESS_INFORMATION pinfo;  // Contains information about a newly created process and its primary thread
    STARTUPINFO         sinfo;  // Specify window station, desktop, standard handles, and appearance of the main window

    WCHAR   CWDPath[MAX_PATH] = { 0 };
    CHAR    HookPath[MAX_PATH] = { 0 };
    DWORD   ExitCode = 0;
    BOOL    bCheck = 0;

    // Create space in memory and fill it with zeros
    ZeroMemory(&sinfo, sizeof(STARTUPINFO));
    ZeroMemory(&pinfo, sizeof(PROCESS_INFORMATION));
    sinfo.cb = sizeof(STARTUPINFO);
    //sinfo.dwFlags = (STARTF_USESHOWWINDOW);
    //sinfo.wShowWindow = SW_SHOW;
    GetCurrentDirectoryW(MAX_PATH, CWDPath);
    GetCurrentDirectoryA(MAX_PATH, HookPath);
    if (Arch == 32)
        strcat_s(HookPath, MAX_PATH, "\\bin\\hook32.dll");
    else
        strcat_s(HookPath, MAX_PATH, "\\bin\\hook64.dll");
    // Create a process with a DLL that will hook all Loadlibrary() calls
    bCheck = DetourCreateProcessWithDllExW(
        NULL,
        pExePath,
        NULL,
        NULL,
        FALSE,
        CREATE_DEFAULT_ERROR_MODE,
        NULL,
        (LPCWSTR)&CWDPath,
        &sinfo,
        &pinfo,
        HookPath,
        NULL);
    if (!bCheck)
        PrintError("DetourCreateProcessWithDllExW()");
    else
    {
        // Start main thread of the process
        ResumeThread(pinfo.hThread);
        // Finish the process after a few seconds
        Sleep(seconds * 1000);
        GetExitCodeProcess(pinfo.hProcess, &ExitCode);
        TerminateProcess(pinfo.hProcess, 0);
        // Ensure that process is finished before continue execution
        WaitForSingleObject(pinfo.hProcess, INFINITE);
    }
    CloseHandle(&sinfo);
    CloseHandle(&pinfo);
    return bCheck;
}

BOOL OpenTemps(FILE** pTmpFileA, FILE** pTmpFileW)
{
    WCHAR	PathFileA[512] = { 0 };
    WCHAR	PathFileW[512] = { 0 };
    LPCWSTR	TmpNameA = L"HookedLLA";
    LPCWSTR	TmpNameW = L"HookedLLW";
    WCHAR	PathBuff[MAX_PATH] = { 0 };
    errno_t errnoA, errnoW;

    GetTempPath(MAX_PATH, PathBuff);
    wcscpy_s(PathFileA, 512, PathBuff);
    wcscat_s(PathFileA, 512, TmpNameA);
    wcscpy_s(PathFileW, 512, PathBuff);
    wcscat_s(PathFileW, 512, TmpNameW);
    
    if (!(PathFileExistsW(PathFileA)) || !(PathFileExistsW(PathFileW)))
    {
        printf("[!] The hooking process didn't found any dinamicaly load DLL for this executable\n");
        return FALSE;
    }
    errnoA = _wfopen_s(pTmpFileA, PathFileA, L"r");
    errnoW = _wfopen_s(pTmpFileW, PathFileW, L"r");
    if (pTmpFileA == NULL ||
        pTmpFileW == NULL ||
        errnoA != 0 ||
        errnoW != 0)
    {
        printf("[!] ERROR: GetExplicitDlls() - _wfopen_s\n");
        return FALSE;
    }
    return TRUE;
}

BOOL DeleteTemps()
{
    WCHAR	PathFileA[512] = { 0 };
    WCHAR	PathFileW[512] = { 0 };
    LPCWSTR	TmpNameA = L"HookedLLA";
    LPCWSTR	TmpNameW = L"HookedLLW";
    WCHAR	PathBuff[MAX_PATH] = { 0 };

    GetTempPath(MAX_PATH, PathBuff);
    wcscpy_s(PathFileA, 512, PathBuff);
    wcscat_s(PathFileA, 512, TmpNameA);
    wcscpy_s(PathFileW, 512, PathBuff);
    wcscat_s(PathFileW, 512, TmpNameW);

    if (!DeleteFileW(PathFileA))
    {
        PrintError("GetExplicitDLLs - DeleteFileW()");
        return FALSE;
    }
    if (!DeleteFileW(PathFileW))
    {
        PrintError("GetExplicitDLLs - DeleteFileW()");
        return FALSE;
    }
    return TRUE;
}

/*
** CheckDLLName has two main functionalities:
** This function appends the substring L".dll" to those DLL names retrieved without extension.
** Also cleans the substring "\\?\" if it is present when a DLL is retrieved as a full path.
** The susbstring "\\?\" is used by some programers to avoid the MAX_PATH limitation.
*/

PCWSTR CheckDLLName(LPCWSTR DLLName)
{
    LPWSTR  NewDLLName;
    size_t  Len;

    if (!DLLName)
        return NULL;
    if (wcslen(DLLName) == 0)
        return NULL;
    if (wcsstr(DLLName, L"\\\\?\\"))
    {
        NewDLLName = _wcsdup(DLLName + 4);
        if (DLLName)
            free((PVOID)DLLName);
        return (LPCWSTR)NewDLLName;
    }
    if (wcsstr(DLLName, L".dll") || wcsstr(DLLName, L".DLL"))
        return DLLName;
    else
    {
        Len = wcslen(DLLName) + 4;
        NewDLLName = (LPWSTR)calloc(Len + 1, sizeof(WCHAR));
        if (!NewDLLName)
            return (NULL);
        wcscpy_s(NewDLLName, Len + 1, DLLName);
        wcscat_s(NewDLLName, Len + 1, L".dll");
        if (DLLName)
            free((PVOID)DLLName);
        return (LPCWSTR)NewDLLName;
    }
}

INT	GetExplicitDlls(ARGS Args, DLLINFO* DLL, INT DLLNum, WORD Arch)
{
    FILE* pTmpFileA = NULL;
    FILE* pTmpFileW = NULL;
    CHAR    ReadBuffA[10000] = { 0 };
    WCHAR   ReadBuffW[10000] = { 0 };
    LPSTR   TokenA, nextTokenA;
    LPWSTR  TokenW, nextTokenW;
    LPWSTR  TmpFlag;
    size_t  read;
    INT     i;

    if (!DLL)
        return DLLNum;
    if (!HookInjector(Args.EPath, Arch, Args.ProcWait))
        return DLLNum;
    if (!OpenTemps(&pTmpFileA, &pTmpFileW))
        return DLLNum;
    i = DLLNum;
    read = fread(ReadBuffA, sizeof(CHAR), 10000, pTmpFileA);
    if (read > 0)
    {
        ReadBuffA[read - 1] = '\0'; //Deletes the last '\n' of the file that leads into an invalid last DLL Name.
        TokenA = strtok_s(ReadBuffA, ",\n", &nextTokenA);
        while (TokenA != NULL)
        {
            DLL[i].Name = AnsiToWide(TokenA);
            DLL[i].Name = CheckDLLName(DLL[i].Name);
            TokenA = strtok_s(NULL, ",\n", &nextTokenA);
            DLL[i].FPath = AnsiToWide(TokenA);
            TokenA = strtok_s(NULL, ",\n", &nextTokenA);
            TmpFlag = AnsiToWide(TokenA); // Hold the Flag value as a string
            if (TmpFlag)
            {
                DLL[i].dFlag = (DWORD)_wtoi(TmpFlag);
                free(TmpFlag);
            }
            TokenA = strtok_s(NULL, ",\n", &nextTokenA);
            DLL[i].Import = AnsiToWide(TokenA);
            TokenA = strtok_s(NULL, ",\n", &nextTokenA);
            i++;
        }
    }
    fclose(pTmpFileA);

    // Same procedure with the file that contains the retrieved wstrings
    read = fread(ReadBuffW, sizeof(WCHAR), 10000, pTmpFileW);
    if (read > 0)
    {
        ReadBuffW[read - 1] = '\0'; //Deletes the last '\n' of the file that leads into an invalid last DLL Name.
        TokenW = wcstok_s(ReadBuffW, L",\n", &nextTokenW);
        while (TokenW != NULL)
        {
            DLL[i].Name = _wcsdup(TokenW);
            DLL[i].Name = CheckDLLName(DLL[i].Name);
            TokenW = wcstok_s(NULL, L",\n", &nextTokenW);
            DLL[i].FPath = _wcsdup(TokenW);
            TokenW = wcstok_s(NULL, L",\n", &nextTokenW);
            TmpFlag = _wcsdup(TokenW);
            if (TmpFlag)
            {
                DLL[i].dFlag = (DWORD)_wtoi(TmpFlag);
                free(TmpFlag);
            }
            TokenW = wcstok_s(NULL, L",\n", &nextTokenW);
            DLL[i].Import = _wcsdup(TokenW);
            TokenW = wcstok_s(NULL, L",\n", &nextTokenW);
            i++;
        }
    }
    fclose(pTmpFileW);
    DeleteTemps();
    return i;
}
