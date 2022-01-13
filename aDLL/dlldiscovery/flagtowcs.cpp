#include "adll.h"

LPCWSTR FlagToWCS(DWORD Flag){

    switch(Flag)
    {
        case 0x00000000:
            return(L"NONE");
        case 0x00000001:
            return(L"DONT_RESOLVE_DLL_REFERENCES");
        case 0x00000010:
            return(L"LOAD_IGNORE_CODE_AUTHZ_LEVEL");
        case 0x00000002:
            return(L"LOAD_LIBRARY_AS_DATAFILE");
        case 0x00000040:
            return(L"LOAD_LIBRARY_AS_DATAFILE_EXCLUSIVE");
        case 0x00000020:
            return(L"LOAD_LIBRARY_AS_IMAGE_RESOURCE");
        case 0x00000200:
            return(L"LOAD_LIBRARY_SEARCH_APPLICATION_DIR");
        case 0x00001000:
            return(L"LOAD_LIBRARY_SEARCH_DEFAULT_DIRS");
        case 0x00000100:
            return(L"LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR");
        case 0x00000800:
            return(L"LOAD_LIBRARY_SEARCH_SYSTEM32");
        case 0x00000400:
            return(L"LOAD_LIBRARY_SEARCH_USER_DIRS");
        case 0x00000008:
            return(L"LOAD_WITH_ALTERED_SEARCH_PATH");
        case 0x00000080:
            return(L"LOAD_LIBRARY_REQUIRE_SIGNED_TARGET");
        case 0x00002000:
            return(L"LOAD_LIBRARY_SAFE_CURRENT_DIRS");                      // This flag can be used if NTDDI_VERSION >= NTDDI_WIN10_RS1
        case 0x00004000:
            return(L"LOAD_LIBRARY_SEARCH_SYSTEM32_NO_FORWARDER");           // This flag can be used if NTDDI_VERSION >= NTDDI_WIN10_RS1
        case 0x00008000:
            return(L"LOAD_LIBRARY_OS_INTEGRITY_CONTINUITY");                // This Flag can be used if NTDDI_VERSION >= NTDDI_WIN10_RS2
        case 0x00001100:
            return(L"(LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR | LOAD_LIBRARY_SEARCH_DEFAULT_DIRS)");
    }
    // If there is a flag combination that has not be considered in the switch the returning string is that combination as an hexadecimal number 
    return(L"UNKNOWN");
}