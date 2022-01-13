#include "adll.h"

/*
** LoadIntoMemory() is the function that loads the image of the executable or DLL
** that is going to be analyzed in the search of imported DLLs. This function returns
** the pointer that is pointing to the begining of the image.
*/

LPVOID LoadIntoMemory(LPCWSTR EPath)
{
    HANDLE  hFile;
    PVOID   virtualptr;
    DWORD   byteread, size;
    PIMAGE_DOS_HEADER dosHeader;

    // Open the file
    hFile = CreateFile(
        EPath,
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        0);
    if (hFile == INVALID_HANDLE_VALUE)
    {
        PrintError("LoadIntoMemory - CreateFile()");
        return NULL;
    }

    // Load the image
    size = GetFileSize(hFile, NULL);
    virtualptr = VirtualAlloc(NULL, size, MEM_COMMIT, PAGE_READWRITE);
    if (!ReadFile(hFile, virtualptr, size, &byteread, NULL))
    {
        PrintError("LoadIntoMemory - ReadFile()");
        CloseHandle(hFile);
        return NULL;
    }

    // First bytes of PE belongs to the MS_DOS section
    dosHeader = (PIMAGE_DOS_HEADER)virtualptr;

    // Check if loaded file is an executable
    if (dosHeader->e_magic != IMAGE_DOS_SIGNATURE)
    {
        printf("[!] ERROR: Selected file is not a windows executable\n");
        CloseHandle(hFile);
        return NULL;
    }
    CloseHandle(hFile);
    return virtualptr;
}