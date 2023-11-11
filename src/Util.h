#pragma once

#include <windows.h>
#include <string>

char* OpenFileSelect();

char* ConvertToChar(wchar_t* in);
wchar_t* ConvertToWchar(const char* in);

//wchar_t* ConvertCharToWchar(const char* in);
//char* ConvertWchatToChar(const wchar_t* in);

void fileWrite(const std::string& path, const std::string& content);
std::string fileRead(const std::string& path);
bool isFileExists_ifstream(const std::string& path);
bool IsDirectory_win(const LPCWSTR path);
bool DeleteDir_win(const LPCWSTR path);
