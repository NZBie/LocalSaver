#pragma once

#include <algorithm>
#include <map>
#include <vector>

#include <json.h>

using namespace std;

enum SaveType
{
	E_auto = 0,
	E_manual,
};

// ���ݴ浵
class GameSave
{
public:
	const int _id;
	const SaveType _type;
	const string _path;
};

// ������Ϸ�浵��
class GameLib
{
public:
	const string _name;
	string _originPath;
	vector<GameSave> _gameSaves;

public:
	GameLib(const string& gameName, const string& originPath);	// �´浵��
	GameLib(const string& gameName);							// ���д浵��
	~GameLib();

	void ReadData();
	void SaveData();
};

class LocalSaver
{
protected:
	vector<const string> _libNames;
	map<const string, GameLib> _gameLibs;

	bool _haveChange = true;

public:
	int getGameCount() { return _gameLibs.size(); }
	vector<const string>& getAllGameLibName(bool& haveChange);
	GameLib* getGamedLibInfo(const string& gameName);
	bool AddGameLib(const string& gameName, const string& originPath);
	void DeleteGameLib(const string& gameName, bool clearSaves);

public:
	void ReadData();
	void SaveData();

protected:
	LocalSaver();
	~LocalSaver();
};

extern LocalSaver $ls;
