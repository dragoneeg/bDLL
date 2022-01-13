// dllmain.cpp : Defines the entry point for the DLL application.
#define _CRT_SECURE_NO_WARNINGS
#include "stdafx.h"
#include <sys/stat.h>
#include <windows.h>
#include <fstream>

using namespace std;

EXTERN_C IMAGE_DOS_HEADER __ImageBase;

bool copyFile(string origin_file_path, string target_file_path)
{
	FILE* infile, * outfile;
	infile = fopen(origin_file_path.c_str(), "rb");
	outfile = fopen(target_file_path.c_str(), "wb");
	if (infile == NULL || outfile == NULL)
		return FALSE;
	const int buffsize = 2048;

	char buff[buffsize];
	int read_count = 0;
	while (!feof(infile))
	{
		read_count = fread(buff, sizeof(char), buffsize, infile);
		fwrite(buff, sizeof(char), read_count, outfile);
	}
	fclose(infile);
	fclose(outfile);
	return TRUE;
}

std::string Exists_Dll(string name)
{
	struct stat buffer;
	int pos = name.find_last_of('\\');
	std::string dllname(name.substr(pos + 1));
	std::string dllname32 = "C:/Windows/System32/" + dllname;
	std::string dllname64 = "C:/Windows/SysWOW64/" + dllname;
	if (stat(dllname32.c_str(), &buffer) == 0)
	{
		if (stat(dllname64.c_str(), &buffer) == 0)
		{
			return(" (In System32 and SysWOW64!)");
		}
		return(" (In System32!)");
	}
	else
	{
		if (stat(dllname64.c_str(), &buffer) == 0)
		{
			return(" (In SysWOW64!)");
		}
		return (" ");
	}
}

std::string TCHAR2STRING(TCHAR* str)

{
	std::string strstr;
	try
	{
		int iLen = WideCharToMultiByte(CP_ACP, 0, str, -1, NULL, 0, NULL, NULL);
		char* chRtn = new char[iLen * sizeof(char)];
		WideCharToMultiByte(CP_ACP, 0, str, -1, chRtn, iLen, NULL, NULL);
		strstr = chRtn;
	}
	catch (std::exception e)
	{
	}
	return strstr;
}

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH: {

		WCHAR  DllPath[MAX_PATH] = { 0 };
		TCHAR  ExePath[1024];
		CHAR   read[MAX_PATH] = { 0 };
		std::string flag;
		//get exe path
		GetModuleFileName(NULL, ExePath, 1024);
		//MessageBox(NULL, filename, L"exe name", MB_ICONINFORMATION);

		//get dll path
		GetModuleFileNameW((HINSTANCE)&__ImageBase, DllPath, __crt_countof(DllPath));
		//MessageBox(NULL, LPCWSTR(DllPath), L"dll name", MB_ICONINFORMATION);

		//in system32|64 ?
		flag = Exists_Dll(TCHAR2STRING(DllPath));

		//write in txt
		string ExePath_Str = TCHAR2STRING(ExePath);
		string::size_type iPos = ExePath_Str.find_last_of('\\') + 1;
		string TempFilePath = ExePath_Str.substr(0, iPos);
		ofstream outfile(TempFilePath +"..\\dlljacking.txt", ios::app);
		outfile << "exe: " << ExePath_Str << "\r\ndll: " << TCHAR2STRING(DllPath) + flag << "\r\n\r\n";
		outfile.close();

		//copyfile
		string GoodExePath = TempFilePath + "..\\Goodexe\\";
		string ExeName = ExePath_Str.substr(iPos, ExePath_Str.length() - iPos);
		GoodExePath += ExeName;
		copyFile(ExePath_Str, GoodExePath);
		exit(0);
		break;
	}
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}
