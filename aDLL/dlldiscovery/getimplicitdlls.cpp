#include "adll.h"

/*
** Implicit Linked DLLs are those modules that are loaded in memory at the same
** time that the executable does.
** This function recieves a pointer to the and searchs through the PE format the
** Import Table that is the section where the names of this implicit linked DLLs
** are stored.
*/

DWORD RVA_to_Offset(DWORD RVA, PIMAGE_SECTION_HEADER pSectH, WORD NumOfSections)
{
    PIMAGE_SECTION_HEADER   p;
    size_t                  i;

    if (RVA == 0)
        return (RVA);
    p = pSectH;
    for (i = 0; i < NumOfSections; i++)
    {
        if (RVA >= p->VirtualAddress && RVA < p->VirtualAddress + p->Misc.VirtualSize)
            break;
        p++;
    }
    return (RVA - p->VirtualAddress + p->PointerToRawData);
}

INT GetImplicitDlls32(LPVOID VirtualPtr, DLLINFO* DLL)
{
    // Punteros para recorrer las secciones del PE
    PIMAGE_DOS_HEADER           pDosHeader;
    PIMAGE_NT_HEADERS32         pNTHeader;
    PIMAGE_SECTION_HEADER       pSectHeader;
    PIMAGE_IMPORT_DESCRIPTOR    pImpDesc;
    PIMAGE_DELAYLOAD_DESCRIPTOR pDelayDesc;
    DWORD ImpDescRVA, ImpDescOffset;

    // Variables used to retrieve DLL names
    LPSTR   DllName[500] = { 0 };
    DWORD   DllRVA, DllOffset;
    INT     i, j;

    // First bytes of PE32/PE32+ belong to MS_DOS section
    pDosHeader = (PIMAGE_DOS_HEADER)VirtualPtr;
    // Get a pointer to the PE Header.
    pNTHeader = (PIMAGE_NT_HEADERS32)((UINT_PTR)VirtualPtr + pDosHeader->e_lfanew);

    // Check that the files has an Import Table
    if (pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].Size == 0)
    {
        printf("[!] ERROR - Selected file doesn't have an Import Table\n");
        return NULL;
    }
    // Get a pointer to the Section Table
    pSectHeader = IMAGE_FIRST_SECTION(pNTHeader);
    // Get a pointer to the Import Table
    ImpDescRVA = pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;
    ImpDescOffset = RVA_to_Offset(ImpDescRVA, pSectHeader, pNTHeader->FileHeader.NumberOfSections);
    pImpDesc = (PIMAGE_IMPORT_DESCRIPTOR)((UINT_PTR)VirtualPtr + ImpDescOffset);
    // Get the number and names of the DLLs (Import Table contains an IMAGE_IMPORT_DESCRIPTOR struct for each DLL)
    i = 0;
    while (pImpDesc->Name != NULL)
    {
        DllRVA = pImpDesc->Name;
        DllOffset = RVA_to_Offset(DllRVA, pSectHeader, pNTHeader->FileHeader.NumberOfSections);
        DllName[i] = (LPSTR)((UINT_PTR)VirtualPtr + DllOffset);
        pImpDesc++;
        i++;
    }
    // Set the DLLINFO struct fields for each implicit linked DLL 
    for (j = 0; j < i; j++)
    {
        DLL[j].Name = AnsiToWide(DllName[j]);
        DLL[j].Import = _wcsdup(L"Linker");
    }

    // Same procedure for delay loaded DLLs
    if (pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT].Size == 0)  // File does not have Delayed DLLs
        return j;
    ImpDescRVA = pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT].VirtualAddress;
    ImpDescOffset = RVA_to_Offset(ImpDescRVA, pSectHeader, pNTHeader->FileHeader.NumberOfSections);
    pDelayDesc = (PIMAGE_DELAYLOAD_DESCRIPTOR)((UINT_PTR)VirtualPtr + ImpDescOffset);
    while (pDelayDesc->DllNameRVA != NULL)
    {
        DllRVA = pDelayDesc->DllNameRVA;
        DllOffset = RVA_to_Offset(DllRVA, pSectHeader, pNTHeader->FileHeader.NumberOfSections);
        DllName[i] = (LPSTR)((UINT_PTR)VirtualPtr + DllOffset);
        pDelayDesc++;
        i++;
    }
    for (j; j < i; j++)
    {
        DLL[j].Name = AnsiToWide(DllName[j]);
        DLL[j].Import = _wcsdup(L"Delayed");
    }
    return j;
}

INT GetImplicitDlls64(LPVOID VirtualPtr, DLLINFO* DLL)
{
    // Punteros para recorrer las secciones del PE
    PIMAGE_DOS_HEADER           pDosHeader;
    PIMAGE_NT_HEADERS64         pNTHeader;
    PIMAGE_SECTION_HEADER       pSectHeader;
    PIMAGE_IMPORT_DESCRIPTOR    pImpDesc;
    PIMAGE_DELAYLOAD_DESCRIPTOR pDelayDesc;
    DWORD ImpDescRVA, ImpDescOffset;

    // Variables used to retrieve DLL names
    LPSTR   DllName[500] = { 0 };
    DWORD   DllRVA, DllOffset;
    INT     i, j;

    // First bytes of PE32/PE32+ belong to MS_DOS section
    pDosHeader = (PIMAGE_DOS_HEADER)VirtualPtr;
    // Get a pointer to the PE Header.
    pNTHeader = (PIMAGE_NT_HEADERS64)((UINT_PTR)VirtualPtr + pDosHeader->e_lfanew);
     
    // Check that the files has an Import Table
    if (pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].Size == 0)
    {
        printf("[!] ERROR - Selected file doesn't have an Import Table\n");
        return NULL;
    }
    // Get a pointer to the Section Table
    pSectHeader = IMAGE_FIRST_SECTION(pNTHeader);
    // Get a pointer to the Import Table
    ImpDescRVA = pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;
    ImpDescOffset = RVA_to_Offset(ImpDescRVA, pSectHeader, pNTHeader->FileHeader.NumberOfSections);
    pImpDesc = (PIMAGE_IMPORT_DESCRIPTOR)((UINT_PTR)VirtualPtr + ImpDescOffset);
    // Get the number and names of the DLLs (Import Table contains an IMAGE_IMPORT_DESCRIPTOR struct for each DLL)
    i = 0;
    while (pImpDesc->Name != NULL)
    {
        DllRVA = pImpDesc->Name;
        DllOffset = RVA_to_Offset(DllRVA, pSectHeader, pNTHeader->FileHeader.NumberOfSections);
        DllName[i] = (LPSTR)((UINT_PTR)VirtualPtr + DllOffset);
        pImpDesc++;
        i++;
    }
    // Set the DLLINFO struct fields for each implicit linked DLL 
    for (j = 0; j < i; j++)
    {
        DLL[j].Name = AnsiToWide(DllName[j]);
        DLL[j].Import = _wcsdup(L"Linker");
    }

    // Same procedure for delay loaded DLLs
    if (pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT].Size == 0)  // File does not have Delayed DLLs
        return j;
    ImpDescRVA = pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT].VirtualAddress;
    ImpDescOffset = RVA_to_Offset(ImpDescRVA, pSectHeader, pNTHeader->FileHeader.NumberOfSections);
    pDelayDesc  = (PIMAGE_DELAYLOAD_DESCRIPTOR)((UINT_PTR)VirtualPtr + ImpDescOffset);
    while (pDelayDesc->DllNameRVA != NULL)
    {
        DllRVA = pDelayDesc->DllNameRVA;
        DllOffset = RVA_to_Offset(DllRVA, pSectHeader, pNTHeader->FileHeader.NumberOfSections);
        DllName[i] = (LPSTR)((UINT_PTR)VirtualPtr + DllOffset);
        pDelayDesc++;
        i++;
    }
    for (j; j < i; j++)
    {
        DLL[j].Name = AnsiToWide(DllName[j]);
        DLL[j].Import = _wcsdup(L"Delayed");
    }
    return j;
}

INT GetImplicitDlls(LPVOID VirtualPtr, DLLINFO* DLL, WORD Arch)
{
    INT DllNum = 0;

    if (Arch == 32)
        DllNum = GetImplicitDlls32(VirtualPtr, DLL);
    else
        DllNum = GetImplicitDlls64(VirtualPtr, DLL);
    return DllNum;
}