#include "autogame.h"

using namespace PhasmoCheatV::Features::Auto;

const std::vector<SDK::ObjectivesType> allObjectives = {
	SDK::ObjectivesType::BlowoutCandle,
	SDK::ObjectivesType::DetectParabolicSound,
	SDK::ObjectivesType::EMFGhost,
	SDK::ObjectivesType::EscapeHunt,
	SDK::ObjectivesType::GhostPhoto,
	SDK::ObjectivesType::IdentifyGhost,
	SDK::ObjectivesType::MotionSensorGhost,
	SDK::ObjectivesType::PreventHunt,
	SDK::ObjectivesType::SanityBelow25,
	SDK::ObjectivesType::SmudgeGhost,
	SDK::ObjectivesType::SmudgeHunt,
	SDK::ObjectivesType::WitnessEvent,
	SDK::ObjectivesType::DnaEvidence,
	SDK::ObjectivesType::CompleteInTimeLimit,
	SDK::ObjectivesType::GhostVideo,
	SDK::ObjectivesType::UniquePhotos,
	SDK::ObjectivesType::UniqueVideos,
	SDK::ObjectivesType::UniqueSounds
};

AutoGame::AutoGame() : FeatureCore(LANG("AutoGame_Header"), TYPE_AUTO)
{
	DECLARE_CONFIG(GetConfigManager(), "AutoGameDelay", int, 5);
	DECLARE_CONFIG(GetConfigManager(), "mapId", int, 4);
}

const char* AutoGame::AutoGamePhaseToString(AutoGame::AutoGamePhase phase)
{
	switch (phase)
	{
	case AutoGamePhase::Idle: return "IDLE";
	case AutoGamePhase::FindLobby: return "FIND LOBBY";
	case AutoGamePhase::Voting: return "VOTING";
	case AutoGamePhase::WaitingForVote: return "WAITING FOR VOTE";
	case AutoGamePhase::StartingGame: return "STARTING GAME";
	case AutoGamePhase::WaitingForPlayers: return "WAITING PLAYERS";
	case AutoGamePhase::WaitingDelayAfterPlayers: return "POST DELAY";
	case AutoGamePhase::CompletingObjectives: return "OBJECTIVES";
	case AutoGamePhase::Exiting: return "EXITING";
	case AutoGamePhase::SkipAndNext: return "SKIP & NEXT";
	case AutoGamePhase::Done: return "DONE";
	default: return "UNKNOWN";
	}
}

void AutoGame::OnDeactivate()
{
	started = false;
	phase = AutoGamePhase::Idle;
	skipDone = false;
	nextDone = false;
	cachedPostGame = nullptr;
	stopFlag = false;
	stopRequested = false;
	stopping = false;
	rewardedTotal = 0;
}

void AutoGame::OnRender()
{
	if (!IsActive())
		return;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16, 16));
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8, 10));
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 6));

	constexpr ImGuiWindowFlags flags =
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_AlwaysAutoResize |
		ImGuiWindowFlags_NoNav;

	ImGui::Begin("###AutoGamePanel", nullptr, flags);

	ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
	ImGui::TextColored(ImVec4(0.2f, 0.8f, 1.0f, 1.0f), "AUTOGAME PANEL");
	ImGui::PopFont();

	ImGui::Separator();
	ImGui::Spacing();

	ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.9f, 1.0f), "Phase:");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(0.4f, 1.0f, 0.4f, 1.0f), "%s", AutoGamePhaseToString(phase));

	ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.9f, 1.0f), "Earned:");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(0.4f, 1.0f, 0.4f, 1.0f), "%d", rewardedTotal);

	ImGui::Spacing();

	if (!started)
	{
		ImGui::BeginDisabled();
		ImGui::Button("Stop AutoGame", ImVec2(200, 30));
		ImGui::EndDisabled();

		if (ImGui::Button("Start AutoGame", ImVec2(200, 35)))
		{
			started = true;
			stopFlag = false;
			stopRequested = false;
			stopping = false;
			phase = AutoGamePhase::Idle;
			rewardedTotal = 0;
			nextActionTime = std::chrono::steady_clock::now();
		}
	}
	else
	{
		ImGui::BeginDisabled();
		ImGui::Button("Start AutoGame", ImVec2(200, 35));
		ImGui::EndDisabled();

		if (stopping)
		{
			ImGui::BeginDisabled();
			ImGui::Button("Stop AutoGame", ImVec2(200, 30));
			ImGui::EndDisabled();

			ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.2f, 1.0f),
				"Stopping / finishing current cycle...");
		}
		else
		{
			if (ImGui::Button("Stop AutoGame", ImVec2(200, 30)))
			{
				stopFlag = true;
				stopping = true;
			}
		}
	}

	ImGui::End();

	ImGui::PopStyleVar(3);
}

void AutoGame::OnMenuRender()
{
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 6));

	bool enabled = IsActive();

	bool canToggle = (!started && phase == AutoGamePhase::Idle && !stopping);

	if (!canToggle && enabled)
		ImGui::BeginDisabled();

	if (ImGui::Checkbox(LANG("EnableAutoGame"), &enabled))
	{
		SET_CONFIG_VALUE(GetConfigManager(), "Enabled", bool, enabled);

		if (enabled)
			OnActivate();
		else
			OnDeactivate();
	}

	if (!canToggle && enabled)
		ImGui::EndDisabled();

	if (!canToggle)
	{
		ImGui::Spacing();
		ImGui::TextColored(ImVec4(1.0f, 0.4f, 0.2f, 1.0f), LANG("AutoGameRunning"));
	}

	if (!enabled)
	{
		ImGui::PopStyleVar();
		return;
	}

	int delay = CONFIG_INT(GetConfigManager(), "AutoGameDelay");
	if (ImGui::SliderInt(LANG("AutoGameDelay"), &delay, 1, 30))
	{
		SET_CONFIG_VALUE(GetConfigManager(), "AutoGameDelay", int, delay);
	}

	int mapIndex = 0;
	int savedMapId = CONFIG_INT(GetConfigManager(), "mapId");

	for (int i = 0; i < IM_ARRAYSIZE(mapIds); i++)
	{
		if (mapIds[i] == savedMapId)
		{
			mapIndex = i;
			break;
		}
	}

	if (ImGui::Combo("Map", &mapIndex, mapItems, IM_ARRAYSIZE(mapItems)))
	{
		SET_CONFIG_VALUE(GetConfigManager(), "mapId", int, mapIds[mapIndex]);
	}

	ImGui::PopStyleVar();
}

void AutoGame::AutoGameHandler()
{
	if (!started || !IsActive())
	{
		phase = AutoGamePhase::Idle;
		return;
	}

	if (stopFlag)
		stopRequested = true;

	int mapIdCfg = CONFIG_INT(GetConfigManager(), "mapId");
	int delayMs = CONFIG_INT(GetConfigManager(), "AutoGameDelay") * 1000;

	auto now = std::chrono::steady_clock::now();

	bool inRoom = SDK::PhotonNetwork_Get_InRoom(nullptr);

	if (!inRoom)
	{
		phase = AutoGamePhase::Idle;
		return;
	}

	if (now < nextActionTime)
		return;

	switch (phase)
	{
	case AutoGamePhase::Idle:
		phase = AutoGamePhase::FindLobby;
		nextActionTime = now + std::chrono::milliseconds(delayMs);
		break;

	case AutoGamePhase::FindLobby:
	{
		auto mainManager = SDK::MainManager_staticFields->instance;
		if (!mainManager)
			return;

		auto levelSelectionManager = mainManager->Fields.levelSelection;
		if (!levelSelectionManager)
			return;

		phase = AutoGamePhase::Voting;
		nextActionTime = now + std::chrono::milliseconds(delayMs);
		break;
	}

	case AutoGamePhase::Voting:
	{
		auto mainManager = SDK::MainManager_staticFields->instance;
		if (!mainManager)
			return;

		auto manager = mainManager->Fields.levelSelection;
		if (!manager)
			return;

		SDK::LevelSelectionManager_VoteMap(manager, mapIdCfg, nullptr);

		phase = AutoGamePhase::WaitingForVote;
		nextActionTime = now + std::chrono::milliseconds(delayMs);
		break;
	}

	case AutoGamePhase::WaitingForVote:
	{
		auto mainManager = SDK::MainManager_staticFields->instance;
		if (!mainManager)
		{
			LOG_ERROR("MainManager instance is null");
			return;
		}

		auto manager = mainManager->Fields.levelSelection;
		if (!manager) {
			LOG_INFO("manager is null");
			return;
		}

		auto* votedContract = manager->Fields.votedContract;
		if (!votedContract)
		{
			LOG_INFO("contract is null");
			return;
		}

		auto* map = votedContract->Fields.info;
		if (!map)
		{
			LOG_INFO("map is null");
			return;
		}

		votedId = map->Fields.uniqueMapID;

		if (votedId == mapIdCfg)
		{
			phase = AutoGamePhase::StartingGame;
			nextActionTime = now + std::chrono::milliseconds(delayMs);
		}
		else
		{
			LOG_INFO("votedId = %d mapId = %d", votedId, mapIdCfg);
			//ret;
		}
		break;
	}

	case AutoGamePhase::StartingGame:
	{
		if (!SDK::PhotonNetwork_Get_IsMasterClient(nullptr))
			return;

		auto mainManager = SDK::MainManager_staticFields->instance;
		if (!mainManager)
		{
			LOG_ERROR("MainManager instance is null");
			return;
		}

		auto* serverManager = mainManager->Fields.serverManager;
		if (!serverManager) return;

		SDK::ServerManager_StartGame(serverManager, nullptr);

		phase = AutoGamePhase::WaitingForPlayers;
		nextActionTime = now + std::chrono::milliseconds(delayMs);
		break;
	}

	case AutoGamePhase::WaitingForPlayers:
	{
		auto* gameController = SDK::GameController_StaticFields->instance;
		if (!gameController) return;

		if (gameController->Fields.allPlayersAreConnected || Utils::GetGhostAI() && Utils::GetGhostAI()->Fields.GhostInfo && Utils::GetGhostAI()->Fields.GhostInfo->Fields.GhostTraits.Name)
		{
			waitStart = now;
			phase = AutoGamePhase::WaitingDelayAfterPlayers;
			break;
		}
		else
			return;
	}

	case AutoGamePhase::WaitingDelayAfterPlayers:
	{
		auto seconds = std::chrono::duration_cast<std::chrono::seconds>(now - waitStart).count();

		if (seconds < 2)
			return;

		phase = AutoGamePhase::CompletingObjectives;
		nextActionTime = now + std::chrono::milliseconds(delayMs);
		break;
	}

	case AutoGamePhase::CompletingObjectives:
	{
		auto* objectiveManager = SDK::ObjectiveManager_StaticFields->instance;
		if (!objectiveManager) return;

		for (auto obj : allObjectives)
			SDK::ObjectiveManager_CompleteObjectiveSynced(objectiveManager, obj, nullptr);

		phase = AutoGamePhase::Exiting;
		nextActionTime = now + std::chrono::milliseconds(delayMs);
		break;
	}

	case AutoGamePhase::Exiting:
	{
		auto* exitGO = Utils::FindObjectByName("Keypad");
		if (!exitGO) return;

		auto* photonView = reinterpret_cast<SDK::PhotonView*>(
			SDK::GameObject_GetComponentByName(exitGO, Utils::SysStrToUnityStr("PhotonView"), nullptr));

		if (!photonView) return;

		SDK::PhotonView_RPC(
			photonView,
			Utils::SysStrToUnityStr("Exit"),
			SDK::RpcTarget::All,
			nullptr,
			nullptr
		);

		phase = AutoGamePhase::SkipAndNext;
		nextActionTime = now + std::chrono::milliseconds(delayMs);
		break;
	}

	case AutoGamePhase::SkipAndNext:
	{
		if (!InGame::rewardManager) return;

		rewardedTotal += InGame::rewardManager->Fields.totalReward;

		SDK::RewardManager_Skip(InGame::rewardManager, nullptr);
		SDK::RewardManager_Next(InGame::rewardManager, nullptr);

		phase = AutoGamePhase::Done;
		nextActionTime = now + std::chrono::milliseconds(delayMs);
		break;
	}

	case AutoGamePhase::Done:
	{
		if (stopRequested)
		{
			started = false;
			stopFlag = false;
			stopRequested = false;
			stopping = false;
			phase = AutoGamePhase::Idle;
			return;
		}

		phase = AutoGamePhase::Idle;
		break;
	}
	}
}