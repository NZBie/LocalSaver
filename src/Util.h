#pragma once

#include <windows.h>
#include <string>

char* OpenFileSelect();

LPSTR ConvertLPWSTRToLPSTR(LPCWSTR lpwszStrIn);
LPWSTR ConvertLPSTRToLPWSTR(LPCSTR lpwszStrIn);

//wchar_t* ConvertCharToWchar(const char* in);
//char* ConvertWchatToChar(const wchar_t* in);

void fileWrite(const std::string& path, const std::string& content);
std::string fileRead(const std::string& path);
bool isFileExists_ifstream(const std::string& path);
