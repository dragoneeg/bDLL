#include "adll.h"

LPWSTR* GetKnownDlls()
{
    HKEY    hKey = NULL; 
    LPWSTR  *KnownDlls = NULL;
    LSTATUS lStat;
    BYTE    ValueData[_MAX_FNAME] = { 0 };
    WCHAR   ValueName[_MAX_FNAME] = { 0 };
    DWORD   NumValues, MaxStrLen, MaxSize;
    DWORD   i, StrLen, Size;

    //Open the registry and get a handle to the Known DLLs subkey
    lStat = RegOpenKeyEx(
        HKEY_LOCAL_MACHINE,
        L"SYSTEM\\CurrentControlSet\\Control\\Session Manager\\KnownDLLs",
        0,
        KEY_READ,
        &hKey);
    if (lStat != ERROR_SUCCESS)
    {
        PrintError("RegOpenKeyEx(): ");
        return NULL;
    }
    // Retrieve information about the Known DLLs subkey
    lStat = RegQueryInfoKeyW(
        hKey,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        &NumValues,         //Number of values associated with the key.
        &MaxStrLen,         //Size of the longest value of the field "name" not including '\0'
        &MaxSize,           //Size of the longest value of the field "data" in bytes.
        NULL,
        NULL);

    if (lStat != ERROR_SUCCESS)
    {
        PrintError("GetKnownDlls - RegOpenKeyEx(): ");
        return NULL;
    }
    if (!(KnownDlls = (LPWSTR*)calloc((SIZE_T)NumValues + 1, sizeof(LPWSTR))))
    {
        printf("[!] ERROR: GetKnownDlls - Calloc couldn't allocate memory");
        return NULL;
    }
    // Iterate over the values of the subkey and get the content of the data field for each value
    for (i = 0; i < NumValues; i++)
    {
        SecureZeroMemory(ValueName, _MAX_FNAME);
        SecureZeroMemory(ValueData, _MAX_FNAME);
        StrLen  = MaxStrLen + 1;
        Size    = MaxSize;
        RegEnumValue(
            hKey,
            i,
            ValueName,  // This parameter is not usefull here but RegEnumValue needs it to work properly
            &StrLen,    // This parameter is not usefull here but RegEnumValue needs it to work properly
            NULL,
            NULL,
            ValueData,  // This buffer is filled with the data field of the subkey. That is the DLL name
            &Size);     // This variable recieve the total number of bytes written in the buffer
        ValueData[MaxSize + 1] = 0;
        //_strlwr_s((char *)ValueData, MaxSize + 1);
        KnownDlls[i] = _wcsdup((PWCHAR)ValueData);
        //printf("Known DLL %d: %ws\n", i, KnownDlls[i]);
    }
    return (KnownDlls);
}
