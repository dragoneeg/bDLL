#include "adll.h"

/*
** This functon creates the reports of analyzed programs
*/

BOOL GenReport(ARGS Args, DLLINFO *DLL)
{
    WCHAR   NewOPath[1024] = { 0 };
    WCHAR   dir[MAX_PATH] = { 0 };
    WCHAR   fname[MAX_PATH] = { 0 };
    WCHAR   Ext[5] = { 0 };
    WCHAR   Num[64] = { 0 };
    WORD    v = 1;

    // Split the output path into three items: directory path, filename and extension of file
    _wsplitpath_s(Args.OPath, NULL, 0, dir, _countof(dir), fname, _countof(fname), Ext, _countof(Ext));
    // If the given path is an existing file the loop add a number to the filename until the new filename does not exist
    wcscpy_s(NewOPath, _countof(NewOPath), Args.OPath);
    while (PathFileExistsW(NewOPath))
    {
        v++;
        wcscpy_s(NewOPath, _countof(NewOPath), dir);
        wcscat_s(NewOPath, _countof(NewOPath), fname);
        _itow_s(v, Num, _countof(Num), 10);
        wcscat_s(NewOPath, _countof(NewOPath), Num);
        wcscat_s(NewOPath, _countof(NewOPath), Ext);
    }

    FILE    *pReport;
    WCHAR   Line[2048];
    INT     i = 0;

    // Create the Report file
    _wfopen_s(&pReport, NewOPath, L"w");
    wcscpy_s(Line, _countof(Line), L"Report for ");
    wcscat_s(Line, _countof(Line), Args.EPath);
    wcscat_s(Line, _countof(Line), L"\n");
    fwrite(Line, sizeof(WCHAR), wcslen((LPCWSTR)Line), pReport);
    // Prepare a line for each DLL and write it into the report file as CSV format
    ZeroMemory(Line, sizeof(Line));
    for (i = 0; DLL[i].Name != NULL; i++)
    {
        wcscpy_s(Line, _countof(Line), DLL[i].Name);
        wcscat_s(Line, _countof(Line), L",");
        wcscat_s(Line, _countof(Line), DLL[i].Import);
        wcscat_s(Line, _countof(Line), L",");
        _itow_s(DLL[i].dFlag, Num, _countof(Num), 10);
        wcscat_s(Line, _countof(Line), Num);
        wcscat_s(Line, _countof(Line), L",");
        _itow_s(DLL[i].Known, Num, _countof(Num), 10);
        wcscat_s(Line, _countof(Line), Num);
        wcscat_s(Line, _countof(Line), L",");
        _itow_s(DLL[i].Found, Num, _countof(Num), 10);
        wcscat_s(Line, _countof(Line), Num);
        wcscat_s(Line, _countof(Line), L",");
        wcscat_s(Line, _countof(Line), DLL[i].FPath);
        wcscat_s(Line, _countof(Line), L"\n");
        fwrite(Line, sizeof(WCHAR), wcslen((LPCWSTR)Line), pReport);
    }
    fclose(pReport);
    return TRUE;
}
