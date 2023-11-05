
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
	ImGui::Begin(u8"�����Ϸ", p_open);
	ImGui::SameLine();

	static char gameName[64] = "";
	ImGui::InputText(u8"��Ϸ����", gameName, IM_ARRAYSIZE(gameName));

	static char originPath[256] = "";
	ImGui::InputText(u8"##�浵·��", originPath, IM_ARRAYSIZE(originPath));
	ImGui::SameLine();
	if (ImGui::Button(u8"�浵·��"))
	{
		char* pathInfo = OpenFileSelect();
		if (pathInfo != NULL)
		{
			strcpy_s(originPath, pathInfo);
		}		
	}

	//static char savePath[256] = "";
	//ImGui::InputText(u8"##����·��", savePath, IM_ARRAYSIZE(savePath));
	//ImGui::SameLine();
	//if (ImGui::Button(u8"����·��"))
	//{
	//	char* pathInfo = OpenFileSelect();
	//	if (pathInfo != NULL)
	//	{
	//		strcpy_s(savePath, pathInfo);
	//	}
	//}

	if (ImGui::Button(u8"ȷ�����"))
	{
		cout << gameName << endl;
		auto x = ConvertLPSTRToLPWSTR(gameName);
		cout << x << endl;
		if ($ls.AddGameLib(gameName, originPath))
		{
			*p_open = false;
		}
	}

	ImGui::End();
}

void DrawGameList()
{
	static int curGameId = 0;
	bool needRefreshLibList = false;
	auto& libNames = $ls.getAllGameLibName(needRefreshLibList);
	static string gameNamesItemStr = "";

	// �����Ϸ��ť
	{
		static bool show_addGame_window = false;
		if (ImGui::Button(u8"�����Ϸ"))
		{
			show_addGame_window = true;
		}
		if (show_addGame_window)
		{
			ShowAddGameWindow(&show_addGame_window);
		}
	}

	// ɾ����Ϸ��ť
	{
		ImGui::SameLine();
		if (ImGui::Button(u8"ɾ������Ϸ"))
		{
			ImGui::OpenPopup("delete_popup");
		}
		if (ImGui::BeginPopup("delete_popup"))
		{
			if (ImGui::Selectable(u8"ɾ���浵�ļ�"))
				$ls.DeleteGameLib(libNames[curGameId], true);
			if (ImGui::Selectable(u8"��ɾ����¼"))
				$ls.DeleteGameLib(libNames[curGameId], false);
			if (ImGui::Selectable(u8"ȡ��"))
				;
			ImGui::EndPopup();
		}
	}

	// ��ǰ��Ϸ�б�
	{
		if (needRefreshLibList)
		{
			gameNamesItemStr.clear();
			for (auto& name : libNames)
			{
				gameNamesItemStr += name + '\0';
			}
		}
		if (ImGui::Combo(u8"��Ϸ�б�", &curGameId, gameNamesItemStr.c_str(), 5))
		{
			$ls.SaveData();
		}
	}

	ImGui::SameLine(); HelpMarker(u8"�浵�����Ϸ");
}

void ShowLocalSaverWindow(bool* p_open)
{
	ImGui::Begin("LocalSaver", p_open);

	$ls.ReadData();

	DrawGameList();

	ImGui::End();
}
