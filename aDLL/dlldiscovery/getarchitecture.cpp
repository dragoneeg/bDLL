#include "adll.h"

WORD GetArchitecture(PVOID VirtualPtr)
{
    PIMAGE_DOS_HEADER dosHeader = NULL;
    PIMAGE_NT_HEADERS pNTHeader = NULL;
    WORD arch = 0;

    if (!VirtualPtr)
        return 0;
    // Los primeros bytes del PE pertenecen a la sección MS_DOS
    dosHeader = (PIMAGE_DOS_HEADER)VirtualPtr;
    // Obtener el puntero al PE Header
    pNTHeader = (PIMAGE_NT_HEADERS)((DWORD_PTR)VirtualPtr + dosHeader->e_lfanew);
    if (pNTHeader->OptionalHeader.Magic == MAGIC_32)
        arch = 32;
    else if (pNTHeader->OptionalHeader.Magic == MAGIC_64)
        arch = 64;
    else
    {
        printf("[!] ERROR: unable to identify the architecture\n");
        arch = 0;
    }
    return arch;
}