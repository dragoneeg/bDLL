#include "adll.h"

VOID PrintVerbose(DLLINFO* DLL)
{
    LPCWSTR Flag = NULL;
    INT     i;

    printf("[+] VERBOSE:\n");
    for (i = 0; DLL[i].Name != NULL; i++)
    {
        Flag = FlagToWCS(DLL[i].dFlag);
        printf("%ws\n{\n\tKnown = %d\n\tImported by %ws\n\tFlag: %d (%ws)\n\tFound (%d) in %ws\n}\n\n",
            DLL[i].Name, DLL[i].Known, DLL[i].Import, DLL[i].dFlag, Flag, DLL[i].Found, DLL[i].FPath);
    }
}
