#include "adll.h"

/*
** This function checks if the DLL is a virtual DLL that has to 
** be redirected by the ApiSetSchema system. At this moment this
** version of aDLL ignores all the virtual DLL because most of them
** redirect to Known DLLs. In a future version of this tool an ApiSet
** redirector may be implemented.
*/

BOOL IsApiSet(LPCWSTR DllName)
{
    if (wcsstr(DllName, L"api-ms") != NULL || wcsstr(DllName, L"ext-ms") != NULL)
        return TRUE;
    return FALSE;
}