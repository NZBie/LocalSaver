#include "LocalSaver.h"
#include "Util.h"
#include "config.h"

#include <iostream>
#include <stdio.h>
#include <windows.h>
#include <Shlwapi.h>

static Json::StyledWriter JsonSW;
static Json::FastWriter JsonFW;
static Json::Reader JsonR;

void GameSave::makeJson(Json::Value& root)
{
	root.clear();
	root["id"] = this->_id;
	root["type"] = this->_type;
	root["path"] = this->_path;
}

GameLib::GameLib(const string& gameName, const string& originPath) :
	_name(gameName),
	_originPath(originPath)
{

}

GameLib::GameLib(const string& gameName)
	: _name(gameName)
{
	ReadData();


}

GameLib::~GameLib()
{
}

bool GameLib::ReadData()
{
	const string filePath = config::savePath + _name + '/' + config::saveDataFileName;
	const string strJson = fileRead(filePath);
	Json::Value root;
	JsonR.parse(strJson, root);
	if (!root.isObject()) return false;

	_originPath = root["originPath"].asString();
	if (root["saves"].isArray())
	{
		for (auto saveJson : root["saves"])
		{
			this->_gameSaves.push_back(
				GameSave
				{
					saveJson["id"].asInt(),
					SaveType(saveJson["type"].asInt()),
					saveJson["path"].asString()
				}
			);
		}
	}
	return true;
}

bool GameLib::SaveData()
{
	Json::Value root;
	root["name"] = _name;
	root["originPath"] = _originPath;
	for (auto& save : this->_gameSaves)
	{
		Json::Value saveJson;
		save.makeJson(saveJson);
		root["saves"].append(saveJson);
	}

	const string strJson = JsonSW.write(root);

	string filePath = config::savePath + _name;
	CreateDirectory(ConvertToWchar(filePath.c_str()), NULL);// todo判断目录存在

	filePath += "/";
	filePath += config::saveDataFileName;
	fileWrite(filePath, strJson);
	return true;
}

bool GameLib::ExcuteArchive(bool isAuto)
{
	int id = rand() % 10000;
	const string path = config::savePath + this->_name + "/" + to_string(id);
	CreateDirectory(ConvertToWchar(path.c_str()), NULL);
	this->_gameSaves.push_back(
		GameSave
		{
			id,
			isAuto ? SaveType::E_auto : SaveType::E_manual,
			path
		}
	);
	this->SaveData();
	return true;
}

/* LocalSaver */

LocalSaver::LocalSaver()
{
	wchar_t exeFile[MAX_PATH];
	GetModuleFileName(NULL, exeFile, MAX_PATH);
	for (int i = wcslen(exeFile); i >= 0; i--)
	{
		if (exeFile[i] == '/' || exeFile[i] == '\\')
			break;
		else
			exeFile[i] = '\0';
	}
	config::savePath = ConvertToChar(exeFile) + string("save/");
	printf("<存储目录: %s>\n", config::savePath.c_str());

	ReadData();
}

LocalSaver::~LocalSaver()
{
}

vector<string>& LocalSaver::getAllGameLibName(bool& haveChange)
{
	haveChange = _haveChange;
	_haveChange = false;
	return _libNames;
}

GameLib* LocalSaver::getGamedLibInfo(const string& gameName)
{
	auto lib = _gameLibs.find(gameName);
	if (lib == _gameLibs.end())
		return nullptr;
	else
		return &(lib->second);
}

bool LocalSaver::AddGameLib(const string& gameName, const string& originPath)
{
	if(_gameLibs.find(gameName) != _gameLibs.end())
		return false;

	GameLib lib(gameName, originPath);
	if (!lib.SaveData())
		return false;

	_gameLibs.insert(make_pair(gameName, lib));
	_libNames.push_back(gameName);
	_haveChange = true;

	SaveData();
}

void LocalSaver::DeleteGameLib(const string& gameName, bool clearSaves)
{
	if (clearSaves)
	{
		const string filePath = string(config::savePath) + string(gameName);
		DeleteDir_win(ConvertToWchar(filePath.c_str()));
	}

	_gameLibs.erase(gameName);
	for (int i = 0; i < _libNames.size(); i++)
	{
		if (_libNames[i] == gameName)
		{
			swap(_libNames[i], _libNames[_libNames.size() - 1]);
			_libNames.pop_back();
			_haveChange = true;
			break;
		}
	}

	SaveData();
}

void LocalSaver::ReadData()
{
	const string filePath = config::savePath + config::libNamesFileName;
	const string strJson = fileRead(filePath);
	Json::Value root;
	JsonR.parse(strJson, root);

	_gameLibs.clear();
	_libNames.clear();
	if (root.isObject() && root["names"].isArray())
	{
		for (auto nameV : root["names"])
		{
			const string name = nameV.asString();
			GameLib lib(name);
			_gameLibs.insert(make_pair(name, lib));
			_libNames.push_back(name);
		}
	}
	_haveChange = true;
}

void LocalSaver::SaveData()
{
	Json::Value root;
	for (auto kv : _gameLibs)
	{
		root["names"].append(kv.first);
	}
	const string strJson = JsonFW.write(root);
	const string filePath = config::savePath + config::libNamesFileName;
	fileWrite(filePath, strJson);
}
