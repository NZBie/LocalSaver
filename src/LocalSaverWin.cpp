
#include "LocalSaverWin.h"
#include "LocalSaver.h"
#include "Util.h"

#include <imgui.h>
#include <iostream>

static void HelpMarker(const char* desc)
{
	ImGui::TextDisabled("(?)");
	if (ImGui::BeginItemTooltip())
	{
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted(desc);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}

void ShowAddGameWindow(bool* p_open)
{
	ImGui::Begin("添加游戏", p_open);
	ImGui::SameLine();

	static char gameName[64] = "";
	ImGui::InputText("游戏名称", gameName, IM_ARRAYSIZE(gameName));

	static char originPath[256] = "";
	ImGui::InputText("##存档路径", originPath, IM_ARRAYSIZE(originPath));
	ImGui::SameLine();
	if (ImGui::Button("存档路径"))
	{
		char* pathInfo = OpenFileSelect();
		if (pathInfo != NULL)
		{
			strcpy_s(originPath, pathInfo);
		}		
	}

	static bool show_addError_win = false;
	if (ImGui::Button("确定添加"))
	{
		auto w_gameName = ConvertToWchar(gameName);
		wcout << w_gameName << endl;
		if ($ls.AddGameLib(gameName, originPath))
		{
			*p_open = false;
			show_addError_win = false;
		}
		else
		{
			show_addError_win = true;
		}
	}
	if (show_addError_win)
	{
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(255, 0, 0, 1), "添加失败");
	}

	ImGui::End();
}

void DrawGameList()
{
	static int curGameId = 0;
	bool needRefreshLibList = false;
	auto& libNames = $ls.getAllGameLibName(needRefreshLibList);
	bool existGameLib = libNames.size() > curGameId;	// 当前是否选中lib库

	string libName = "";
	GameLib* gameLib = nullptr;
	if(existGameLib)
	{
		libName = libNames[curGameId];
		gameLib = $ls.getGamedLibInfo(libName);
	}
	
	// 添加游戏按钮
	{
		static bool show_addGame_window = false;
		if (ImGui::Button("添加游戏"))
		{
			show_addGame_window = true;
		}
		if (show_addGame_window)
		{
			ShowAddGameWindow(&show_addGame_window);
		}
	}
	
	// 删除游戏按钮
	bool isDeleteLib = false;
	if (existGameLib)
	{
		ImGui::SameLine();
		if (ImGui::Button("删除该游戏"))
		{
			ImGui::OpenPopup("delete_popup");
		}
		if (ImGui::BeginPopup("delete_popup"))
		{
			if (ImGui::Selectable("删除存档文件"))
			{
				isDeleteLib = true;
			}
			//if (ImGui::Selectable("仅删除记录"))
			//	$ls.DeleteGameLib(libName, false);
			if (ImGui::Selectable("取消"))
				;
			ImGui::EndPopup();
		}
	}

	// 当前游戏列表
	{
		static string gameNamesItemStr = "";
		if (needRefreshLibList)
		{
			gameNamesItemStr.clear();
			for (auto& name : libNames)
			{
				gameNamesItemStr += name + '\0';
			}
		}
		if (ImGui::Combo("游戏列表", &curGameId, gameNamesItemStr.c_str(), 5))
		{

		}
		ImGui::SameLine(); HelpMarker("存档库的游戏");
	}

	// 游戏存档区
	if (existGameLib)
	{
		ImGui::Text(("游戏存档路径: " + gameLib->_originPath).c_str());
		if (ImGui::Button("手动存档"))
		{
			gameLib->ExcuteArchive(false);
		}

		// 存档列表
		for (auto& save : gameLib->_gameSaves)
		{
			ImGui::Text(to_string(save._id).c_str());
		}
	}

	// 逻辑处理
	if (isDeleteLib)
	{
		$ls.DeleteGameLib(libName, true);
	}
}

void ShowLocalSaverWindow(bool* p_open)
{
	ImGui::Begin("LocalSaver", p_open);

	static bool isFirst = true;
	if (isFirst)
	{
		isFirst = false;
		// TODO
		srand(time(0));
	}

	DrawGameList();

	ImGui::End();
}
