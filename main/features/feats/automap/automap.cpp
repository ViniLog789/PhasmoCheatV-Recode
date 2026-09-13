#include "automap.h"

using namespace PhasmoCheatV::Features::Auto;

AutoMap::AutoMap() : FeatureCore(LANG("AutoMap_Header"), TYPE_AUTO)
{
	DECLARE_CONFIG(GetConfigManager(), "SelectedMap", std::string, "");
}

void AutoMap::OnActivate()
{
	hasVoted = false;
	contracts.clear();
	selectedIndex = 0;
	UpdateContracts();
}

void AutoMap::OnDeactivate()
{
	contracts.clear();
	selectedIndex = 0;
	hasVoted = false;
}

void AutoMap::OnMenuRender()
{
	bool enabled = IsActive();
	if (ImGui::Checkbox(LANG("EnableAutoMap"), &enabled))
	{
		SET_CONFIG_VALUE(GetConfigManager(), "Enabled", bool, enabled);
		enabled ? OnActivate() : OnDeactivate();
	}

	if (!enabled)
		return;

	ImGui::Separator();

	if (contracts.empty())
	{
		ImGui::TextDisabled(LANG("Contracts_NotLoaded"));
		return;
	}

	if (selectedIndex < 0 || selectedIndex >= static_cast<int>(contracts.size()))
		selectedIndex = 0;

	std::vector<const char*> items;
	items.reserve(contracts.size());
	for (const auto& c : contracts)
		items.push_back(c.name.c_str());

	if (ImGui::Combo(LANG("SelectMap"), &selectedIndex, items.data(), static_cast<int>(items.size())))
	{
		SET_CONFIG_VALUE(GetConfigManager(), "SelectedMap", std::string, contracts[selectedIndex].name);
		hasVoted = false;
	}

	ImGui::Text(LANG("TestMap_Crash")); // im lazy to fix this, just a warning for now
}

void AutoMap::UpdateContracts()
{
	contracts.clear();

	auto* mainManager = SDK::MainManager_staticFields->instance;
	if (!mainManager)
		return;

	auto* levelSelection = mainManager->Fields.levelSelection;
	if (!levelSelection || !levelSelection->Fields.contracts)
		return;

	auto* array = reinterpret_cast<SDK::ContractsArray*>(levelSelection->Fields.contracts);
	if (!array || array->max_length <= 0)
		return;

	const std::string savedMap = CONFIG_STRING(GetConfigManager(), "SelectedMap");

	for (int32_t i = 0; i < array->max_length; ++i)
	{
		auto* contract = array->vector[i];
		if (!contract || !contract->Fields.info || !contract->Fields.info->Fields.mapName)
			continue;

		std::string name = Utils::UnityStrToSysStr(*contract->Fields.info->Fields.mapName);
		contracts.push_back({ contract, name });

		if (!savedMap.empty() && name == savedMap)
			selectedIndex = static_cast<int>(contracts.size()) - 1;
	}
}

void AutoMap::TryAutoVote()
{
	if (contracts.empty() || selectedIndex < 0 || selectedIndex >= static_cast<int>(contracts.size()))
		return;

	auto* target = contracts[selectedIndex].ptr;
	if (!target)
		return;

	if (!target->Fields.unlocked)
		return;

	auto* mainManager = SDK::MainManager_staticFields->instance;
	if (!mainManager)
		return;

	auto* levelSelection = mainManager->Fields.levelSelection;
	if (!levelSelection)
		return;

	if (levelSelection->Fields.votedContract == target)
	{
		hasVoted = true;
		return;
	}

	SDK::Contract_Vote(target, nullptr);
	hasVoted = true;

	NOTIFY_INFO_QUICK("AutoMap: voted for " + contracts[selectedIndex].name);
	LOG_INFO("AutoMap: voted for \"" + contracts[selectedIndex].name + "\"");
}

void AutoMap::AutoMapHandler()
{
	if (!IsActive())
		return;

	if (Utils::IsInGame())
	{
		hasVoted = false;
		return;
	}

	const float now = SDK::Time_Get_Time(nullptr);
	if (contracts.empty() || (now - lastUpdateTime) > 2.5f)
	{
		UpdateContracts();
		lastUpdateTime = now;
		hasVoted = false;
	}

	if (!hasVoted && Utils::GetLocalPlayer() && SDK::PhotonNetwork_Get_InRoom(nullptr))
		TryAutoVote();
}