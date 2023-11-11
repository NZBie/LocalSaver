
#include "Util.h"

#include <windows.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <json.h>
#include <io.h>
#include <direct.h>

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
		auto path = ConvertToChar(ofn.lpstrFile);
		std::wcout << path << std::endl;
		return path;
	}
	else
	{
		printf("user cancelled\n");
		return NULL;
	}
}

char* ConvertToChar(wchar_t* in)
{
	char* pszOut = nullptr;
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

wchar_t* ConvertToWchar(const char* in)
{
	wchar_t* out = nullptr;
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

void fileWrite(const std::string& path, const std::string& content)
{
	std::ofstream ofile(path);
	if (ofile.good())
	{
		ofile << content;
		ofile.close();
		//printf("[SaveFile: %s]\n%s\n/end/\n", path.c_str(), content);
		printf("[SaveFile: %s]\n", path.c_str());
	}
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

		//printf("[ReadFile: %s]\n%s\n/end/\n", path.c_str(), buff);
		printf("[ReadFile: %s]\n", path.c_str());
		std::string str = buff;
		delete buff;
		return str;
	}
	else
	{
		return "";
	}
}

bool IsDirectory_win(const LPCWSTR path)
{
	wchar_t curPath[MAX_PATH];
	ZeroMemory(curPath, sizeof(curPath));
	swprintf_s(curPath, L"%s//*", path);
	WIN32_FIND_DATAW FindFileData;
	ZeroMemory(&FindFileData, sizeof(WIN32_FIND_DATAA));

	HANDLE hFile = FindFirstFileW(curPath, &FindFileData);
	FindClose(hFile);
	return hFile != INVALID_HANDLE_VALUE;
}

bool DeleteDir_win(const LPCWSTR path)
{
	wchar_t curPath[MAX_PATH];
	ZeroMemory(curPath, sizeof(curPath));
	swprintf_s(curPath, L"%s//*.*", path);

	WIN32_FIND_DATAW FindFileData;
	ZeroMemory(&FindFileData, sizeof(WIN32_FIND_DATAW));
	HANDLE hFile = FindFirstFileW(curPath, &FindFileData);

	wchar_t wstr[MAX_PATH];
	bool ret = true;
	while (ret)
	{
		if (wcscmp(FindFileData.cFileName, L".") && wcscmp(FindFileData.cFileName, L".."))
		{
			swprintf_s(wstr, L"%s/%s", path, FindFileData.cFileName);
			if (IsDirectory_win(wstr))
			{
				printf("Ŀ¼Ϊ:%s/n", wstr);
				ret &= DeleteDir_win(wstr);
			}
			else
			{
				ret &= DeleteFileW(wstr);
			}
		}
		ret &= FindNextFileW(hFile, &FindFileData);
	}
	ret &= FindClose(hFile);
	ret &= RemoveDirectoryW(path);
	return ret;
}

