#include "adll.h"

VOID FreeArgs(ARGS* Args)
{
    if (Args->EPath)
        free((PVOID)Args->EPath);
    if (Args->TPath)
        free((PVOID)Args->TPath);
    if (Args->DPath)
        free((PVOID)Args->DPath);
    if (Args->OPath)
        free((PVOID)Args->OPath);
    return;
}

BOOL aDLL(ARGS Args)
{
    SCHPATHS    SPaths = { 0 };
    DLLINFO     *DLL = NULL;
    LPVOID      VirtualPtr = NULL;
    LPWSTR*     KnownDLLs = NULL;
    LPWSTR      NotFoundBuff, VerboseBuff, pManifest;
    WORD        Arch;
    INT         DLLNum, i;

    printf("\n[+] Analyzing %ws\n", Args.EPath, Args.Recursion);
    if (Args.Manifest)
    {
        if (GetManifest(Args, &pManifest))
            printf("[+] Manifest:\n%ws\n", pManifest);
        else
            printf("[+] The image may not have a manifest or it could be an external file\n");
    }
    if (!(NotFoundBuff = (LPWSTR)calloc(BUFFSIZE, sizeof(WCHAR))))
    {
        printf("[!] ERROR: Calloc() couldn't allocate memory\n");
        return FALSE;
    }

    // Load the image of the executable into memory and get the architecture (32 or 64-bit)
    VirtualPtr = LoadIntoMemory(Args.EPath);
    if (!(Arch = GetArchitecture(VirtualPtr)))
       return -1;

    // Initialize structures
    InitSchPaths(&SPaths, Args.EPath, Arch);
    DLL = InitDLLInfo(500);

    // Get all the imported DLLs from the binary
    DLLNum = GetImplicitDlls(VirtualPtr, DLL, Arch);
    DLLNum = GetExplicitDlls(Args, DLL, DLLNum, Arch);
    // For all the retrieved DLLs check which one is part of the Known DLLs
    for (i = 0; i < DLLNum; i++)
        DLL[i].Known = IsKnownDLL(DLL[i].Name);
    // Check which of all imported DLLs are not found
    GetNotFound(DLL, SPaths, DLLNum, NotFoundBuff);

    // Search DLLs imported by other DLLs
    if (Args.Recursion > 0)
        for (i = 0; i < DLLNum; i++)
            RecursiveSearch(Args.Recursion, DLL[i], SPaths, Arch, NotFoundBuff);

    // Print the DLLs that are not found
    if (wcslen(NotFoundBuff) > 0)
        printf("[+] NOT FOUND DLLs for %ws\n%ws\n", Args.EPath, NotFoundBuff);
    else
        printf("[-] aDLL did not found vulnerable DLLs for %ws. Use -v to get info about the founded DLLs\n", Args.EPath);

    if (Args.Verbose)
        PrintVerbose(DLL);
    if (Args.OPath)
        GenReport(Args, DLL);
    if (Args.Auto || Args.DPath)
        HijackingTest(Args, Arch, NotFoundBuff);
    // Free pointers
    if (NotFoundBuff)
        free((PVOID)NotFoundBuff);
    if (VirtualPtr)
        VirtualFree(VirtualPtr, sizeof(VirtualPtr), MEM_DECOMMIT);
    return TRUE;
}

INT wmain(INT argc, LPCWSTR argv[])
{
    ARGS Args = { 0 };

    //PrintBanner();
    if (!ArgParser(argc, argv, &Args))
    {
        FreeArgs(&Args);
        printf("Use aDLL -h to get the help menu\n");
        return -1;
    }
    if (Args.Help)
    {
        FreeArgs(&Args);
        PrintHelp();
        return 1;
    }
    if (Args.EPath)
    {
        aDLL(Args);
    }
    else if (Args.TPath != 0)
    {
        // In this case the user has specify a txt file with a list of executables
        FILE*   pTFile = NULL;
        CHAR    ReadBuff[10000] = { 0 };
        LPSTR   Token, nextToken;
        size_t  read;
        errno_t errcheck;
        
        // Open the txt file and read the content into a buffer
        errcheck = _wfopen_s(&pTFile, Args.TPath, L"r");
        if (pTFile == NULL || errcheck != 0)
        {
            printf("[!] _wfopen_s error");
            return -1;
        }
        read = fread(ReadBuff, sizeof(CHAR), 10000, pTFile);
        ReadBuff[read] = '\0';
        fclose(pTFile);

        // Split the lines and call aDLL for every executable path
        Token = strtok_s(ReadBuff, "\n", &nextToken);
        while (Token != NULL)
        {
            Args.EPath = AnsiToWide(Token);
            Token = strtok_s(NULL, "\n", &nextToken);
            aDLL(Args);
            Sleep(500);
            if (Args.EPath)
                free((PVOID)Args.EPath);
        }
    }
    //FreeArgs(&Args);
    return 0;
}