#include "adll.h"

VOID SetDLLFields(DLLINFO DLL, DLLINFO* ReDLL, LPCWSTR Path)
{
    if (ReDLL->Import)
        free((PVOID)ReDLL->Import);
    ReDLL->Import = _wcsdup(DLL.Name);                  // Change the string "Linker" for the name of the DLL which is loading the recursive DLL
    ReDLL->Known = IsKnownDLL(ReDLL->Name);             // Set which recursive DLLs belong to the KnownDLLs list
    ReDLL->ReLvl = 1;                                   // Set the recursion level of the DLL
    ReDLL->dFlag = DLL.dFlag;                           // Most LoadLibraryEx Flags have inheritance to the recursive DLL search
    return ;
}

BOOL RecursiveSearch(DWORD recursion, DLLINFO DLL, SCHPATHS SPaths, WORD Arch, LPWSTR NotFoundBuff)
{
    DLLINFO *ReDLL = NULL;
    LPVOID  VirtualPtr = NULL;
    INT     i, DLLNum;

    if (recursion < 1)
        return FALSE;
    printf("RECURSION over %ws (FOUND = %d) (KNOWN = %d) (APISET = %d)\n", DLL.Name, DLL.Found, DLL.Known, IsApiSet(DLL.Name));
    
    // When a DLL is not found or it is a Known DLL there is no interest in searching dependencies
    if (DLL.Found == 0 || DLL.Known == 1)
        return FALSE;
    // If the DLL is a virtual DLL there is no searched for dependencies
    if ((IsApiSet(DLL.Name)))
        return FALSE;
    // Create an array of DLLINFO structures and initialize it
    ReDLL =  InitDLLInfo(100);
    // Get those DLLs that are present in the Import Table of the DLL passed as argument
    VirtualPtr  = LoadIntoMemory(DLL.FPath);
    DLLNum      = GetImplicitDlls(VirtualPtr, ReDLL, Arch);
    if (DLLNum == 0)
        return FALSE;
    // Set struct fields for every recursive loaded DLL
    for (i = 0; i < DLLNum; i++)
    {
        printf("\t %ws\n", ReDLL[i].Name);
        SetDLLFields(DLL, &(ReDLL[i]), DLL.FPath);
    }
    GetNotFound(ReDLL, SPaths, DLLNum, NotFoundBuff);
    // Recursive call to the function until recursion level is 0
    for (i = 0; i < DLLNum; i++)
        RecursiveSearch(recursion - 1, ReDLL[i], SPaths, Arch, NotFoundBuff);
    return TRUE;
}