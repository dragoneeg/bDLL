#include "adll.h"

/*
** This function checks if a DLL belongs to the Known DLL list of the system.
** The static variable is filled with the names of the Known DLLs the first time
** the function is called and the static variable holds that value for the follwing
** calls. 
*/

BOOL IsKnownDLL(LPCWSTR DLLName)
{
    static LPWSTR* KnownDLL = NULL;

    WCHAR   LowerKnownDLL[255] = { 0 };
    WCHAR   LowerDLLName[255] = { 0 };
    INT     i = 0;

    // The first time this function is called the static variable is initialize
    if (KnownDLL == NULL)
        KnownDLL = GetKnownDlls();

    // Copy the name of the DLL and make it lowercase
    wcscpy_s(LowerDLLName, DLLName);
    _wcslwr_s(LowerDLLName, 255);

    // Copy the name of each Known DLL and make it lowercase to compare it with the DLL name
    while (KnownDLL[i])
    {
        wcscpy_s(LowerKnownDLL, KnownDLL[i]);
        _wcslwr_s(LowerKnownDLL, 255);
        if (wcscmp(LowerDLLName, LowerKnownDLL) == 0)
            return TRUE;
        i++;
    }
    return FALSE;
}