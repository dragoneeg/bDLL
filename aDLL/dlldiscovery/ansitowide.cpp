#include "adll.h"

/*
** This function takes a standard ANSI string and returns the
** equivalent Unicode wide string
*/

LPWSTR AnsiToWide(LPCSTR AnsiStr)
{
    LPWSTR wString = NULL;
    size_t wLen, convertedChars;
    errno_t err;

    if (!AnsiStr)
        return NULL;
    wLen = strlen(AnsiStr) + 1;
    if (wLen == 1)
        return (_wcsdup(L""));
    if (!(wString = (LPWSTR)calloc(sizeof(WCHAR), wLen)))
        return NULL;
    err = mbstowcs_s(&convertedChars, wString, wLen, AnsiStr, _TRUNCATE);
    if (err == STRUNCATE)
        printf("/!\\ AnsiToWide(): String \" %s \" was truncated when converting to wide chars\n", AnsiStr);
    return (wString);
}
