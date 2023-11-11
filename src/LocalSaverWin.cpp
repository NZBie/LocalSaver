
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
	ImGui::Begin(u8"添加游戏", p_open);
	ImGui::SameLine();

	static char gameName[64] = "";
	ImGui::InputText(u8"游戏名称", gameName, IM_ARRAYSIZE(gameName));

	static char originPath[256] = "";
	ImGui::InputText(u8"##存档路径", originPath, IM_ARRAYSIZE(originPath));
	ImGui::SameLine();
	if (ImGui::Button(u8"存档路径"))
	{
		char* pathInfo = OpenFileSelect();
		if (pathInfo != NULL)
		{
			strcpy_s(originPath, pathInfo);
		}		
	}

	//static char savePath[256] = "";
	//ImGui::InputText(u8"##保存路径", savePath, IM_ARRAYSIZE(savePath));
	//ImGui::SameLine();
	//if (ImGui::Button(u8"保存路径"))
	//{
	//	char* pathInfo = OpenFileSelect();
	//	if (pathInfo != NULL)
	//	{
	//		strcpy_s(savePath, pathInfo);
	//	}
	//}

	static bool show_addError_win = false;
	if (ImGui::Button(u8"确定添加"))
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
		ImGui::TextColored(ImVec4(255, 0, 0, 1), u8"添加失败");
	}

	ImGui::End();
}

void DrawGameList()
{
	static int curGameId = 0;
	bool needRefreshLibList = false;
	auto& libNames = $ls.getAllGameLibName(needRefreshLibList);
	static string gameNamesItemStr = "";

	// 添加游戏按钮
	{
		static bool show_addGame_window = false;
		if (ImGui::Button(u8"添加游戏"))
		{
			show_addGame_window = true;
		}
		if (show_addGame_window)
		{
			ShowAddGameWindow(&show_addGame_window);
		}
	}
	
	// 删除游戏按钮
	{
		ImGui::SameLine();
		if (ImGui::Button(u8"删除该游戏"))
		{
			ImGui::OpenPopup("delete_popup");
		}
		if (ImGui::BeginPopup("delete_popup"))
		{
			if (ImGui::Selectable(u8"删除存档文件"))
				$ls.DeleteGameLib(libNames[curGameId], true);
			//if (ImGui::Selectable(u8"仅删除记录"))
			//	$ls.DeleteGameLib(libNames[curGameId], false);
			if (ImGui::Selectable(u8"取消"))
				;
			ImGui::EndPopup();
		}
	}

	// 当前游戏列表
	{
		if (needRefreshLibList)
		{
			gameNamesItemStr.clear();
			for (auto& name : libNames)
			{
				gameNamesItemStr += name + '\0';
			}
		}
		if (ImGui::Combo(u8"游戏列表", &curGameId, gameNamesItemStr.c_str(), 5))
		{

		}
	}

	ImGui::SameLine(); HelpMarker(u8"存档库的游戏");
}

void ShowLocalSaverWindow(bool* p_open)
{
	ImGui::Begin("LocalSaver", p_open);

	static bool isFirst = true;
	if (isFirst)
	{
		isFirst = false;
		// TODO
	}

	DrawGameList();

	ImGui::End();
}
