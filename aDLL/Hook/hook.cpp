#include <windows.h>
#include <stdio.h>
#include "..\\detours\\detours.h"

#ifdef _WIN32
#pragma comment(lib, "..\\detours\\lib.X86\\detours.lib")
#endif

#ifdef _WIN64
#pragma comment(lib, "..\\detours\\lib.X64\\detours.lib")
#endif

__declspec(dllexport) VOID CALLBACK DetourFinishHelperProcess() {}

HMODULE(WINAPI* pLLA)(LPCSTR) = LoadLibraryA;
HMODULE(WINAPI* pLLW)(LPCWSTR) = LoadLibraryW;
HMODULE(WINAPI* pLLExA)(LPCSTR, HANDLE, DWORD) = LoadLibraryExA;
HMODULE(WINAPI* pLLExW)(LPCWSTR, HANDLE, DWORD) = LoadLibraryExW;

// Global variables used by every detoured function
FILE*	g_pTmpFileA = NULL;
FILE*	g_pTmpFileW = NULL;
HANDLE	g_hMutex = INVALID_HANDLE_VALUE;

BOOL OpenTemps()
{
	WCHAR	PathFileA[MAX_PATH] = { 0 };
	WCHAR	PathFileW[MAX_PATH] = { 0 };
	LPCWSTR	TmpNameA = L"HookedLLA";
	LPCWSTR	TmpNameW = L"HookedLLW";

	WCHAR	PathBuff[MAX_PATH] = { 0 };
	errno_t errnoA, errnoW;

	GetTempPath(MAX_PATH, PathBuff);
	wcscpy_s(PathFileA, MAX_PATH, PathBuff);
	wcscat_s(PathFileA, MAX_PATH, TmpNameA);
	wcscpy_s(PathFileW, MAX_PATH, PathBuff);
	wcscat_s(PathFileW, MAX_PATH, TmpNameW);
	errnoA = _wfopen_s(&g_pTmpFileA, PathFileA, L"a");
	if (g_pTmpFileA == NULL || errnoA != 0)
	{
		return FALSE;
	}
	errnoW = _wfopen_s(&g_pTmpFileW, PathFileW, L"a");
	if (g_pTmpFileW == NULL || errnoW != 0)
	{
		return FALSE;
	}
	return TRUE;
}

VOID CloseTemps()
{
	if (g_pTmpFileA != NULL)
		fclose(g_pTmpFileA);
	if (g_pTmpFileW != NULL)
		fclose(g_pTmpFileW);
}

/* 
** This function is called by every hooked LoadLibrary function
** to write in the temporary file the name of the DLL that is 
** trying to load and the flag that is being used if any.
*/

BOOL WriteToFile(LPCVOID Text, DWORD CharType)
{
	size_t	Len, Size, Written;

	if (g_pTmpFileA == NULL || g_pTmpFileW == NULL)
		return FALSE;
	if (CharType == 'A')
	{
		Len = strlen((LPCSTR)Text);
		Size = sizeof(CHAR);
		Written = fwrite(Text, Size, Len, g_pTmpFileA);
	}
	else
	{
		Len = wcslen((LPCWSTR)Text);
		Size = sizeof(WCHAR);
		Written = fwrite(Text, Size, Len, g_pTmpFileW);
	}
	if (Written == 0)
		return FALSE;
	return TRUE;
}

HMODULE WINAPI DetouredLLA(LPCSTR lpLibFileName)
{

	DWORD dwWait = WaitForSingleObject(g_hMutex, INFINITE);
	if (dwWait == WAIT_OBJECT_0)
	{
		CHAR	LineBuff[512] = { 0 };
		CHAR	FullPath[MAX_PATH] = { 0 };

		//MessageBoxA(NULL, "LoadLibraryA", lpLibFileName, MB_OK);
        Sleep(100);
		strcpy_s(LineBuff, _countof(LineBuff), lpLibFileName);
		strcat_s(LineBuff, _countof(LineBuff), ",");
		strcat_s(LineBuff, _countof(LineBuff), "Null");
		strcat_s(LineBuff, _countof(LineBuff), ",");
		strcat_s(LineBuff, _countof(LineBuff), "null");
		strcat_s(LineBuff, _countof(LineBuff), ",LoadLibraryA\n");
		OpenTemps();
		WriteToFile((LPCVOID)LineBuff, 'A');
		CloseTemps();
		ReleaseMutex(g_hMutex);
	}
	return pLLA(lpLibFileName);
}

HMODULE WINAPI DetouredLLW(LPCWSTR lpLibFileName)
{
	WCHAR	LineBuff[512] = { 0 };
	WCHAR	FullPath[MAX_PATH] = { 0 };
	size_t	BuffSize;

	DWORD dwWait = WaitForSingleObject(g_hMutex, INFINITE);
	if (dwWait == WAIT_OBJECT_0)
	{
		//MessageBoxW(NULL, L"LoadLibraryW", lpLibFileName, MB_OK);
        Sleep(100);
		BuffSize = sizeof(LineBuff) / sizeof(WCHAR);
		wcscpy_s(LineBuff, BuffSize, lpLibFileName);
		wcscat_s(LineBuff, BuffSize, L",");
		wcscat_s(LineBuff, BuffSize, L"Null");
		wcscat_s(LineBuff, BuffSize, L",");
		wcscat_s(LineBuff, BuffSize, L"null");
		wcscat_s(LineBuff, BuffSize, L",LoadLibraryW\n");
		OpenTemps();
		WriteToFile((LPCVOID)LineBuff, 'W');
		CloseTemps();
		ReleaseMutex(g_hMutex);
	}
	return pLLW(lpLibFileName);
}

HMODULE WINAPI DetouredLLExA(LPCSTR lpLibFileName, HANDLE hFile, DWORD dwFlags)
{
	//HMODULE hModule;
	CHAR	FlagStr[128] = { 0 };
	CHAR	LineBuff[512] = { 0 };
	CHAR	FullPath[MAX_PATH] = { 0 };

	DWORD dwWait = WaitForSingleObject(g_hMutex, INFINITE);
	if (dwWait == WAIT_OBJECT_0)
	{
		//MessageBoxA(NULL, "LoadLibraryExA", lpLibFileName, MB_OK);
        Sleep(100);
		_ultoa_s(dwFlags, FlagStr, _countof(FlagStr), 10);
		strcpy_s(LineBuff, _countof(LineBuff), lpLibFileName);
		strcat_s(LineBuff, _countof(LineBuff), ",");
		strcat_s(LineBuff, _countof(LineBuff), "Null");
		strcat_s(LineBuff, _countof(LineBuff), ",");
		strcat_s(LineBuff, _countof(LineBuff), FlagStr);
		strcat_s(LineBuff, _countof(LineBuff), ",LoadLibraryExA\n");
		OpenTemps();
		WriteToFile((LPCVOID)LineBuff, 'A');
		CloseTemps();
		ReleaseMutex(g_hMutex);
	}
	return pLLExA(lpLibFileName, hFile, dwFlags);
}

HMODULE WINAPI DetouredLLExW(LPCWSTR lpLibFileName, HANDLE hFile, DWORD dwFlags)
{
	WCHAR	FlagStr[128] = { 0 };
	WCHAR	LineBuff[512] = { 0 };
	WCHAR	FullPath[MAX_PATH] = { 0 };
	size_t	BuffSize;

	DWORD dwWait = WaitForSingleObject(g_hMutex, INFINITE);
	if (dwWait == WAIT_OBJECT_0)
	{
		//MessageBoxW(NULL, L"LoadLibraryExW", lpLibFileName, MB_OK);
        Sleep(100);
		_ultow_s(dwFlags, FlagStr, _countof(FlagStr), 10);
		BuffSize = sizeof(LineBuff) / sizeof(WCHAR);
		wcscpy_s(LineBuff, BuffSize, lpLibFileName);
		wcscat_s(LineBuff, BuffSize, L",");
		wcscat_s(LineBuff, BuffSize, L"Null");
		wcscat_s(LineBuff, BuffSize, L",");
		wcscat_s(LineBuff, BuffSize, FlagStr);
		wcscat_s(LineBuff, BuffSize, L",LoadLibraryExW\n");
		OpenTemps();
		WriteToFile((LPCVOID)LineBuff, 'W');
		CloseTemps();
		ReleaseMutex(g_hMutex);
	}
	return pLLExW(lpLibFileName, hFile, dwFlags);
}

BOOL WINAPI DllMain(HINSTANCE hinst, DWORD dwReason, LPVOID reserved)
{
	UNREFERENCED_PARAMETER(hinst);
	UNREFERENCED_PARAMETER(reserved);
	LONG check;

	switch (dwReason)
	{
		case DLL_PROCESS_ATTACH:
		{
			//MessageBoxA(NULL, "Attach", "Hooked", MB_OK);
			g_hMutex = CreateMutex(NULL, FALSE, NULL);
			DetourRestoreAfterWith();
			DetourTransactionBegin();
			DetourUpdateThread(GetCurrentThread());
			DetourAttach(&(PVOID&)pLLA, DetouredLLA);
			DetourAttach(&(PVOID&)pLLW, DetouredLLW);
			DetourAttach(&(PVOID&)pLLExA, DetouredLLExA);
			DetourAttach(&(PVOID&)pLLExW, DetouredLLExW);
			check = DetourTransactionCommit();
			if (check != NO_ERROR)
				return FALSE;
			break;
		}
		case DLL_PROCESS_DETACH:
		{
			CloseTemps();
			if (g_hMutex != INVALID_HANDLE_VALUE)
				CloseHandle(g_hMutex);
			DetourRestoreAfterWith();
			DetourTransactionBegin();
			DetourUpdateThread(GetCurrentThread());
			DetourDetach(&(PVOID&)pLLA, DetouredLLA);
			DetourDetach(&(PVOID&)pLLW, DetouredLLW);
			DetourDetach(&(PVOID&)pLLExA, DetouredLLExA);
			DetourDetach(&(PVOID&)pLLExW, DetouredLLExW);
			check = DetourTransactionCommit();
			if (check != NO_ERROR)
				return FALSE;
			break;
		}
	}
	return TRUE;
}