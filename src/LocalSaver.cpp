#include "LocalSaver.h"
#include "Util.h"
#include "config.h"

#include <iostream>
#include <stdio.h>
#include <windows.h>

LocalSaver $ls;

static Json::StyledWriter JsonSW;
static Json::FastWriter JsonFW;
static Json::Reader JsonR;

GameLib::GameLib(const string& gameName, const string& originPath) :
	_name(gameName),
	_originPath(originPath)
{
	SaveData();
}

GameLib::GameLib(const string& gameName)
	: _name(gameName)
{
	ReadData();


}

GameLib::~GameLib()
{
}

void GameLib::ReadData()
{
	const string filePath = config::savePath + _name + '/' + config::saveDataFileName;
	const string strJson = fileRead(filePath);
	Json::Value root;
	JsonR.parse(strJson, root);

	_originPath = root["originPath"].asString();
}

void GameLib::SaveData()
{
	Json::Value root;
	root["name"] = _name;
	root["originPath"] = _originPath;
	const string strJson = JsonSW.write(root);

	string filePath = config::savePath + _name;
	CreateDirectory(ConvertLPSTRToLPWSTR(filePath.c_str()), NULL);

	filePath += "/";
	filePath += config::saveDataFileName;
	fileWrite(filePath, strJson);
}

/* LocalSaver */

LocalSaver::LocalSaver()
{
}

LocalSaver::~LocalSaver()
{
}
//
//vector<const string>& getAllGameLibName()
//{
//	int id = 0;
//	for (auto& kv : _gameLibs)
//	{
//		const char* name = kv.first.c_str();
//		strcpy_s(names[id], strlen(name), name);
//		id++;
//	}
//}


vector<const string>& LocalSaver::getAllGameLibName(bool& haveChange)
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

	_gameLibs.insert(make_pair(gameName, GameLib(gameName, originPath)));
	_libNames.push_back(gameName);
	_haveChange = true;

	SaveData();
}

void LocalSaver::DeleteGameLib(const string& gameName, bool clearSaves)
{
	_gameLibs.erase(gameName);
	for (int i = 0; i < _libNames.size(); i++)
	{
		if (_libNames[i] == gameName)
		{
			swap(_libNames[i], _libNames[_libNames[i].size() - 1]);
			_libNames.pop_back();
			_haveChange = true;
		}
	}
	if (clearSaves)
	{
		const string filePath = string(config::savePath) + string(gameName);
		RemoveDirectory(ConvertLPSTRToLPWSTR(filePath.c_str()));
	}

	SaveData();
}

void LocalSaver::ReadData()
{
	const string filePath = config::savePath + config::libNamesFileName;
	const string strJson = fileRead(filePath);
	Json::Value root;
	JsonR.parse(strJson, root);

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
