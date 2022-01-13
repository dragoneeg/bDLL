#include "adll.h"

BOOL ArgParser(INT argc, LPCWSTR argv[], ARGS* Args)
{
    INT i;

    if (argc < 2 || argc > 12)
    {
        printf("Invalid number of arguments\n");
        return FALSE;
    }
    for (i = 1; i < argc; i++)
    {
        if (!wcscmp(argv[i], L"-h") || !wcscmp(argv[i], L"--help"))
        {
            Args->Help = TRUE;
            return (TRUE);
        }
        else if (!wcscmp(argv[i], L"-e") || !wcscmp(argv[i], L"--executable-path"))
        {
            i++;
            if (!(argv[i]) || !(PathFileExistsW(argv[i])))
            {
                printf("[!] Argument ERROR: -e --executable-path needs a valid path\n");
                return (FALSE);
            }
            Args->EPath = _wcsdup(argv[i]);
        }
        else if (!wcscmp(argv[i], L"-t") || !wcscmp(argv[i], L"--txtfile-path"))
        {
            i++;
            if (!(PathFileExistsW(argv[i])))
            {
                printf("[!] Argument ERROR: -t --txtfile-path needs a valid path\n");
                return (FALSE);
            }
            Args->TPath = _wcsdup(argv[i]);
        }
        else if (!wcscmp(argv[i], L"-d") || !wcscmp(argv[i], L"--dllcustom-path"))
        {
            i++;
            if (!(PathFileExistsW(argv[i])))
            {
                printf("[!] Argument ERROR: -d --dllcustom-path needs a valid path\n");
                return (FALSE);
            }
            Args->DPath = _wcsdup(argv[i]);
        }
        else if (!wcscmp(argv[i], L"-o") || !wcscmp(argv[i], L"--output-path"))
        {
            i++;
            if (!(argv[i]))
            {
                printf("[!] Argument ERROR: -o --output-path needs a valid path\n");
                return (FALSE);
            }
            Args->OPath = _wcsdup(argv[i]);
        }
        else if (!(wcscmp(argv[i], L"-r")) || !(wcscmp(argv[i], L"--recursion")))
        {
            i++;
            if (!(argv[i]))
            {
                printf("[!] Argument ERROR: -r --recursion flag needs a numeric argument\n");
                return (FALSE);
            }
            Args->Recursion = _wtoi(argv[i]);
        }
        else if (!wcscmp(argv[i], L"-w") || !wcscmp(argv[i], L"--wait"))
        {
            i++;
            if (!(argv[i]))
            {
                printf("[!] Argument ERROR: -w --wait flag needs a numeric argument\n");
                return (FALSE);
            }
            Args->ProcWait = _wtoi(argv[i]);
        }
        else if (!wcscmp(argv[i], L"-m") || !wcscmp(argv[i], L"--manifest"))
            Args->Manifest = TRUE;
        else if (!wcscmp(argv[i], L"-a") || !wcscmp(argv[i], L"--automatic"))
            Args->Auto = TRUE;
        else if (!wcscmp(argv[i], L"-v") || !wcscmp(argv[i], L"--verbose"))
            Args->Verbose = TRUE;
        else
        {
            printf("[!] Argument ERROR: %ws is not a valid flag\n", argv[i]);
            return FALSE;
        }
    }
    // If user did not use one of -e or -t flags or the user did use both at the same time
    if ((!Args->EPath && !Args->TPath) || (Args->EPath && Args->TPath))
    {
        printf("[!] Argument ERROR : You must specify a valid executable path OR a valid txt with a list of executable paths\n");
        return FALSE;
    }
    if (Args->ProcWait <= 0 || Args->ProcWait >= 60)
        Args->ProcWait = 20;
    if (Args->Recursion < 0 || Args->Recursion > 3)
        Args->Recursion = 1;
    return (TRUE);
}