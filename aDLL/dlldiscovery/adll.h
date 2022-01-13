#ifndef ADLL_H
# define ADLL
# include <windows.h>
# include <stdio.h>
# include <shlwapi.h>
# include <wow64apiset.h>
# define MAGIC_32 0x10b
# define MAGIC_64 0x20b
# define BUFFSIZE 10000
# define PATHENVMAX 4096

/******************************************** STRUCTURES *********************************************/

struct ARGS
{
	LPWSTR	EPath;		// Path to the executable file
	LPWSTR	DPath;		// Path to a custom DLL file to test DLL Hijacking 
	LPWSTR	TPath;		// Path to a txt file with a list of executables
	LPWSTR	OPath;		// Path to a directory to save the report as a txt file
	INT		Recursion;	// Recursivity level to search DLLs that are loaded by other DLLs
	INT		ProcWait;	// Amount of seconds to wait before force to terminate the hooked process
	BOOL	Manifest;	// Print the manifest of the executable
	BOOL	Auto;		// Activate an automatic DLL search order hijacking test for the program
	//BOOL	EArgPath;	// Activate parsing of arguments for the executable path.
	BOOL	Verbose;	// Print the list of DLL and all the propierties of each DLL
	//BOOL	VVerbose;	// Print the complete list of DLLs and propierties. Even virtual DLLs.
	BOOL	Help;		// Print the help menu
};

// This structure holds paths in wich a DLL will be searched
struct SCHPATHS
{
	LPCWSTR AppDir;		// Path for the application directory
	LPCWSTR SysDir32;	// Path for system 32 bit directory
	LPCWSTR SysDir16;	// Path for system 16 bit directory
	LPCWSTR WinDir;		// Path for windows directory
	LPCWSTR CWDir;		// Path for the current directory
	LPCWSTR PATHDirs;	// %PATH% Enviroment variable value. Paths are separated by ';'
	LPCWSTR ExtraDir;
};

// This structure holds all the retrieved info of a DLL
struct DLLINFO
{
	LPCWSTR	Name;		// Name of the DLL
	LPCWSTR	Import;		// Way of import e.g. by "Linker" or by "LoadLibrary"
	DWORD	dFlag;		// Decimal number representing the flag of LoadLibraryEx() function
	LPCWSTR	pFlag;		// Flag parsed as a wcstring
	BOOL	Known;		// Tells if the DLL belong to the Known DLLs list
	BOOL	Found;		// Tells if the DLL has been found in disk or not
	LPCWSTR	FPath;		// Path where the DLL has been found
    LPCWSTR IPath;      // Path that will be inherited by the dependencies of the DLL
	WORD	ReLvl;		// Level of recursion when the DLL has been imported by other DLL
	DLLINFO *ReDLLs;	// Pointer to an array of DLLs that are dependencies of the DLL
};

/******************************************** FUNCTIONS **********************************************/

VOID	PrintBanner(VOID);
VOID	PrintHelp(VOID);
VOID	PrintError(LPCSTR message);
VOID	PrintVerbose(DLLINFO* DLL);
DLLINFO	*InitDLLInfo(size_t size);
VOID	InitSchPaths(SCHPATHS* SPaths, LPCWSTR fPath, WORD Arch);
BOOL	ArgParser(INT argc, LPCWSTR argv[], ARGS* Args);
LPWSTR	AnsiToWide(LPCSTR AnsiStr);
LPCWSTR FlagToWCS(DWORD Flag);
LPVOID	LoadIntoMemory(LPCWSTR EPath);
WORD	GetArchitecture(PVOID VirtualPtr);
BOOL	GetManifest(ARGS Args, LPWSTR* pManifest);
INT		GetImplicitDlls(LPVOID VirtualPtr, DLLINFO* DLL, WORD Arch);
INT		GetExplicitDlls(ARGS Args, DLLINFO* DLL, INT DLLNum, WORD Arch);
LPWSTR	*GetKnownDlls(VOID);
BOOL	IsKnownDLL(LPCWSTR DLLName);
BOOL    IsApiSet(LPCWSTR DllName);
VOID	GetNotFound(DLLINFO* DLL, SCHPATHS SPaths, INT DLLNum, LPWSTR NotFoundBuff);
BOOL    HijackingTest(ARGS Args, WORD Arch, LPWSTR NotFoundBuff);
BOOL	GenReport(ARGS Args, DLLINFO* DLL);
BOOL	RecursiveSearch(DWORD recursion, DLLINFO DLL, SCHPATHS SPaths, WORD Arch, LPWSTR NotFoundBuff);

#endif
