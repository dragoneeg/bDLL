#include "adll.h"

VOID PrintError(LPCSTR message)
{
	printf("[!] ERROR: %s failed. GetLastError = %d\n", message, GetLastError());
	return;
}