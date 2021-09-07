#include "GetSpectators.h"
#include "imgui/imgui.h"

std::list<int> ShowSpectators::GetObservervators(int playerId)
{
	std::list<int> list;

	if (!g_EngineClient->IsInGame())
		return list;

	C_BasePlayer* player = (C_BasePlayer*)g_EntityList->GetClientEntity(playerId);
	if (!player)
		return list;

	if (!player->IsAlive())
	{
		C_BasePlayer* observerTarget = (C_BasePlayer*)g_EntityList->GetClientEntityFromHandle(player->m_hObserverTarget());
		if (!observerTarget)
			return list;

		player = observerTarget;
	}

	for (int i = 1; i < g_EngineClient->GetMaxClients(); i++)
	{
		C_BasePlayer* pPlayer = (C_BasePlayer*)g_EntityList->GetClientEntity(i);
		if (!pPlayer)
			continue;

		if (pPlayer->IsDormant() || pPlayer->IsAlive())
			continue;

		C_BasePlayer* target = (C_BasePlayer*)g_EntityList->GetClientEntityFromHandle(pPlayer->m_hObserverTarget());
		if (player != target)
			continue;

		list.push_back(i);
	}

	return list;
}
void ShowSpectators::RenderWindow()
{
	if (!g_Options.spectatorlist)
		return;

	int specs = 0;
	int modes = 0;
	std::string spect = u8"";
	std::string mode = u8"";
	int DrawIndex = 1;

	for (int playerId : GetObservervators(g_EngineClient->GetLocalPlayer()))
	{
		C_BasePlayer* pPlayer = (C_BasePlayer*)g_EntityList->GetClientEntity(playerId);

		if (playerId == g_EngineClient->GetLocalPlayer())
			continue;

		if (!pPlayer)
			continue;

		player_info_t Pinfo;

		g_EngineClient->GetPlayerInfo(playerId, &Pinfo);

		if (Pinfo.fakeplayer)
			continue;

		spect += Pinfo.szName;
		spect += u8"\n";
		specs++;

	}
	bool misc_Spectators = true;

	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 1.f, 1.f, 1.f));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
	if (ImGui::Begin("Spectator List", &g_Options.spectatorlist, ImVec2(), 1.f, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize))
	{
		ImGui::PopStyleVar();
		ImGui::PopStyleColor();

		if (specs > 0) spect += u8"\n";     /*ЛАСТ*/
		if (modes > 0) mode += u8"\n";
		ImVec2 size = ImGui::CalcTextSize(spect.c_str());

		ImGui::SetWindowSize(ImVec2(200, 25 + size.y));
		ImGui::Text(spect.c_str());
		DrawIndex++;
	}
	ImGui::End();
}