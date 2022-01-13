#include "adll.h"

VOID PrintHelp()
{
    puts(
        "Usage:\n"
        "---------------------------------------------------------------------------------------------\n"
        "-e, --executable-path: Path to the executable to be analyzed                                 |\n"
        "-t, --txtfile-path:    Path to a txt file with a list of executable paths to be analyzed     |\n"
        "-o, --output-path:     Path to a directory where a txt file report will be saved             |\n"
        "-d, --dllcustom-path:  Path to a custom dll to use with the automatic search order hijacking |\n"
        "-w, --wait:            Time in seconds to wait before close an analyzed executable process   |\n"
        "-r, --recursion:       Level of recursion to search DLLs loaded by other DLLs. Defoult is 1  |\n"
        "-m, --manifest:        Print the manifest of the executable                                  |\n"
        "-a, --automatic:       Activate the automatic search order hijacking test functionality      |\n"
        "-v, --verbose:         Print all imported DLLs list and all the propierties for each DLL     |\n"
        "---------------------------------------------------------------------------------------------\n"
        "Example of use:\n"
        "aDLL -m -e C:\\Windows\\system32\\notepad.exe"
    );
    return;
}