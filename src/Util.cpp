
#include "Util.h"

#include <windows.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <json.h>

char* OpenFileSelect()
{
	OPENFILENAME ofn;
	char szFile[300];
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;
	ofn.lpstrFile = (LPWSTR)szFile;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = L"All\0*.*\0Text\0*.TXT\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
	if (GetOpenFileName(&ofn))
	{
		auto path = ConvertLPWSTRToLPSTR(ofn.lpstrFile);
		std::cout << ofn.lpstrFile << std::endl;
		std::cout << path << std::endl;
		return path;
	}
	else
	{
		printf("user cancelled\n");
		return NULL;
	}
}

LPSTR ConvertLPWSTRToLPSTR(LPCWSTR in)
{
	LPSTR pszOut = nullptr;
	try
	{
		if (in != nullptr)
		{
			int nInputStrLen = wcslen(in);
			int out = WideCharToMultiByte(CP_ACP, 0, in, nInputStrLen, NULL, 0, 0, 0) + 2;
			pszOut = new char[out];
			if (pszOut)
			{
				memset(pszOut, 0x00, out);
				WideCharToMultiByte(CP_ACP, 0, in, nInputStrLen, pszOut, out, 0, 0);
			}
		}
	}
	catch (std::exception e)
	{
	}
	return pszOut;
}

LPWSTR ConvertLPSTRToLPWSTR(LPCSTR in)
{
	LPWSTR out = nullptr;
	try
	{
		if (in != nullptr)
		{
			int nInputStrLen = MultiByteToWideChar(CP_ACP, 0, in, -1, NULL, 0);
			out = new wchar_t[nInputStrLen];
			MultiByteToWideChar(CP_ACP, 0, in, -1, out, nInputStrLen);
		}
	}
	catch (std::exception e)
	{
	}
	return out;
}

//wchar_t* ConvertCharToWchar(const char* in)
//{
//	int len = strlen(in);
//	wchar_t* out = new wchar_t[len];
//	swprintf(out, len, L"%hs", in);
//	return out;
//}
//
//char* ConvertWchatToChar(const wchar_t* in)
//{
//	int len = wcslen(in);
//	char* out = new char[len];
//	sprintf_s(out, len, "%ws", in);
//	return out;
//}

void fileWrite(const std::string& path, const std::string& content)
{
	std::ofstream ofile(path);
	ofile << content;
	ofile.close();
	printf("[SaveFile: %s]\n%s\n/end/\n", path.c_str(), content);
}

std::string fileRead(const std::string& path)
{
	std::ifstream ifile(path);
	if (ifile.good())
	{
		ifile.seekg(0, std::ios::end);
		int len = ifile.tellg();
		ifile.seekg(0, std::ios::beg);
		char* buff = new char[len + 1];
		ifile.read(buff, len);
		buff[len] = '\0';
		ifile.close();

		printf("[ReadFile: %s]\n%s\n/end/\n", path.c_str(), buff);
		std::string str = buff;
		delete buff;
		return str;
	}
	else
	{
		return "";
	}
}

//bool isFileExists_ifstream(const std::string& path)
//{
//	std::ifstream f(path);
//	return f.good();
//}
