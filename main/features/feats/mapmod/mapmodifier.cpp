#include "mapmodifier.h"
using namespace PhasmoCheatV::Features::Map;

MapModifier::MapModifier() : FeatureCore(LANG("MapModifier_Header"), TYPE_MAP)
{
    DECLARE_CONFIG(GetConfigManager(), "CustomMaxLight", bool, false);
    DECLARE_CONFIG(GetConfigManager(), "MaxLight", int32_t, 10);
    //DECLARE_CONFIG(GetConfigManager(), "AutoSelectMap", bool, false);
    //DECLARE_CONFIG(GetConfigManager(), "AutoVoteMap", std::string, "");
}

void MapModifier::OnMenuRender()
{
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 6));

    bool enabled = IsActive();
    if (ImGui::Checkbox(LANG("EnableMapModifier"), &enabled))
    {
        SET_CONFIG_VALUE(GetConfigManager(), "Enabled", bool, enabled);
        enabled ? OnActivate() : OnDeactivate();
    }

    if (!enabled)
    {
        ImGui::PopStyleVar();
        return;
    }

    ImGui::Separator();

    std::string CurrentMapName = Utils::GetMapName();
    ImGui::Text(LANG("CurrentMap"), CurrentMapName.c_str());

    /*
    bool AutoSelectMap = CONFIG_BOOL(GetConfigManager(), "AutoSelectMap");

    if (ImGui::Checkbox(LANG("AutoSelectMap"), &AutoSelectMap))
        SET_CONFIG_VALUE(GetConfigManager(), "AutoSelectMap", bool, AutoSelectMap);

    if (AutoSelectMap)
    {
        if (contracts.empty())
            ImGui::TextDisabled("No contracts available");
        else
        {
            if (selectedAutoVoteContract >= static_cast<int>(contracts.size()))
                selectedAutoVoteContract = 0;

            std::vector<const char*> mapItems;
            mapItems.reserve(contracts.size());

            for (const auto& contract : contracts)
                mapItems.push_back(contract.contract_name.c_str());

            if (ImGui::Combo(
                LANG("SelectMap"),
                &selectedAutoVoteContract,
                mapItems.data(),
                static_cast<int>(mapItems.size())))
            {
                SET_CONFIG_VALUE(
                    GetConfigManager(),
                    "AutoVoteMap",
                    std::string,
                    contracts[selectedAutoVoteContract].contract_name
                );
            }
        }
    }
    */

    ImGui::Separator();

    bool CustomMaxLight = CONFIG_BOOL(GetConfigManager(), "CustomMaxLight");
    int32_t MaxLight = CONFIG_INT(GetConfigManager(), "MaxLight");

    if (ImGui::Checkbox(LANG("CustomMaxLights"), &CustomMaxLight))
        SET_CONFIG_VALUE(GetConfigManager(), "CustomMaxLight", bool, CustomMaxLight);

    if (CustomMaxLight)
    {
        if (ImGui::SliderInt(LANG("MaxLights"), &MaxLight, 1, 100))
            SET_CONFIG_VALUE(GetConfigManager(), "MaxLight", int32_t, MaxLight);
    }

    if (ImGui::Button(LANG("ActivateAllLights")))
        lightsModifier = 1;

    ImGui::SameLine();

    if (ImGui::Button(LANG("DeactivateAllLights")))
        lightsModifier = 2;

    if (ImGui::Button(LANG("TriggerLightning")))
        callLightning = true;

    ImGui::SameLine();

    if (ImGui::Button(LANG("SwitchFuseBox")))
        switchFuseBox = true;

    ImGui::PopStyleVar();
}

void MapModifier::MapModifierMain()
{
    if (IsActive() && CONFIG_BOOL(GetConfigManager(), "CustomMaxLight") && SDK::LevelController_sFields->instance && SDK::LevelController_sFields->instance->Fields.fuseBox)
    {
        SDK::LevelController_sFields->instance->Fields.fuseBox->Fields.maxLights = CONFIG_INT(GetConfigManager(), "MaxLight");
    }

    /*

    if (IsActive() && CONFIG_BOOL(GetConfigManager(), "AutoSelectMap"))
    {
        auto* mainManager = SDK::MainManager_staticFields->instance;
        auto* levelSelection = mainManager ? mainManager->Fields.levelSelection : nullptr;

        if (levelSelection && levelSelection->Fields.contracts)
        {
            std::string targetMapName = CONFIG_STRING(GetConfigManager(), "AutoVoteMap");

            if (!targetMapName.empty())
            {
                auto* array = reinterpret_cast<SDK::ContractsArray*>(levelSelection->Fields.contracts);
                SDK::Contract* targetContract = nullptr;

                for (int32_t i = 0; i < array->max_length; ++i)
                {
                    auto* contract = array->vector[i];
                    if (!contract || !contract->Fields.info || !contract->Fields.info->Fields.mapName)
                        continue;

                    std::string currentName = Utils::UnityStrToSysStr(*contract->Fields.info->Fields.mapName);
                    if (currentName == targetMapName)
                    {
                        targetContract = contract;
                        break;
                    }
                }

                if (targetContract && levelSelection->Fields.votedContract != targetContract)
                {
                    LOG_INFO("AutoVote: Voting for " + targetMapName);
                    SDK::Contract_Vote(targetContract, nullptr);

                    NOTIFY_INFO_QUICK("Auto-selected map: " + targetMapName);
                }
            }
        }
    }

    */
    
    if (IsActive() && lightsModifier == 1)
    {
        lightsModifier = false;

        auto vectorLights = InGame::lightSwitchs;

        if (vectorLights.empty())
        {
            NOTIFY_ERROR_QUICK(LANG("NeedToBeInGame"));
            return;
        }

        for (SDK::LightSwitch* lightSwitch : vectorLights)
        {
            if (!lightSwitch)
                continue;

            SDK::LightSwitch_Use(lightSwitch, true, false, false, false, nullptr);
        }

        NOTIFY_SUCCESS_QUICK(LANG("AllLightsActivated"));
    }
    if (IsActive() && lightsModifier == 2)
    {
        lightsModifier = 0;

        auto vectorLights = InGame::lightSwitchs;

        if (vectorLights.empty())
        {
            NOTIFY_ERROR_QUICK(LANG("NeedToBeInGame"));
            return;
        }

        for (SDK::LightSwitch* lightSwitch : vectorLights)
        {
            if (!lightSwitch)
                continue;

            SDK::LightSwitch_Use(lightSwitch, false, false, false, false, nullptr);
        }

        NOTIFY_SUCCESS_QUICK(LANG("AllLightsDeactivated"));
    }
    if (IsActive() && callLightning)
    {
        callLightning = false;

        auto* lightningController = InGame::lightningController;

        if (!lightningController)
        {
            NOTIFY_ERROR_QUICK(LANG("NeedToBeInGame"));
            return;
        }

        auto* randomWeather = SDK::RandomWeather_sFields->instance;

        if (!randomWeather)
        {
            NOTIFY_ERROR_QUICK(LANG("NeedToBeInGame"));
            return;
        }

        auto* weatherProfile = randomWeather->Fields.currentWeatherProfile;

        if (!weatherProfile)
        {
            NOTIFY_ERROR_QUICK(LANG("NeedToBeInGame"));
            return;
        }

        auto gameObject = SDK::Component_Get_GameObject(reinterpret_cast<SDK::Component*>(lightningController), nullptr);
        if (!gameObject)
        {
            NOTIFY_ERROR_QUICK(LANG("NeedToBeInGame"));
            return;
        }

        auto photonView = reinterpret_cast<SDK::PhotonView*>(SDK::GameObject_GetComponentByName(gameObject, Utils::SysStrToUnityStr("Photon.Pun.PhotonView"), nullptr));
        if (!photonView)
        {
            NOTIFY_ERROR_QUICK(LANG("NeedToBeInGame"));
            return;
        }

        if (weatherProfile->Fields.weatherType != SDK::WeatherType::heavyRain) //! NEVER REMOVE THIS IF LOOP
        {
            NOTIFY_ERROR_QUICK(LANG("WeatherShouldBeHeavyRain"));
            return;
        }

        if (!SDK::PhotonNetwork_Get_IsMasterClient(nullptr) || !SDK::PhotonNetwork_Get_OfflineMode(nullptr)) //! NEVER REMOVE THIS IF LOOP
        {
            NOTIFY_ERROR_QUICK("NeedMustBeHost");
            return;
        }

        SDK::PhotonView_RPC(photonView, Utils::SysStrToUnityStr("PlayLightningNetworked"), SDK::RpcTarget::All, nullptr, nullptr);

        NOTIFY_SUCCESS_QUICK(LANG("LightningTriggered"));
    }

    if (IsActive() && switchFuseBox)
    {
        switchFuseBox = false;

        auto levelController = SDK::LevelController_sFields->instance;
        if (!levelController)
        {
            NOTIFY_ERROR_QUICK(LANG("NeedToBeInGame"));
            return;
        }

        auto* fuseBox = levelController->Fields.fuseBox;

        if (!fuseBox)
        { 
            NOTIFY_ERROR_QUICK(LANG("NeedToBeInGame"));
            return;
        }

        SDK::FuseBox_Use(fuseBox, nullptr);

        NOTIFY_SUCCESS_QUICK(LANG("FuseBoxSwitched"));
    }
}

/*
std::vector<MapModifier::ContractsRet> MapModifier::GetAllContracts()
{
    std::vector<MapModifier::ContractsRet> contracts;

    auto mainManager = SDK::MainManager_staticFields->instance;
    if (!mainManager)
    {
        LOG_ERROR("MapModifier::GetAllContracts: MainManager instance is null");
        return contracts;
    }
    LOG_INFO("MapModifier::GetAllContracts: MainManager OK");

    auto levelSelection = mainManager->Fields.levelSelection;
    if (!levelSelection)
    {
        LOG_ERROR("MapModifier::GetAllContracts: levelSelection is null");
        return contracts;
    }
    LOG_INFO("MapModifier::GetAllContracts: levelSelection OK");

    auto array = reinterpret_cast<SDK::ContractsArray*>(levelSelection->Fields.contracts);
    if (!array)
    {
        LOG_ERROR("MapModifier::GetAllContracts: contracts array is null");
        return contracts;
    }

    LOG_INFO("MapModifier::GetAllContracts: contracts array max_length = " + std::to_string(array->max_length));

    if (array->max_length <= 0)
    {
        LOG_ERROR("MapModifier::GetAllContracts: contracts array is empty (max_length <= 0)");
        return contracts;
    }

    int validCount = 0;
    int nullContract = 0;
    int nullInfo = 0;
    int nullMapName = 0;

    for (int32_t i = 0; i < array->max_length; ++i)
    {
        auto contract = array->vector[i];
        if (!contract)
        {
            ++nullContract;
            continue;
        }
        if (!contract->Fields.info)
        {
            ++nullInfo;
            continue;
        }

        auto mapName = contract->Fields.info->Fields.mapName;
        if (!mapName)
        {
            ++nullMapName;
            continue;
        }

        std::string name = Utils::UnityStrToSysStr(*mapName);
        contracts.push_back({
            contract,
            name
            });
        ++validCount;

        LOG_INFO("MapModifier::GetAllContracts: [" + std::to_string(i) + "] " + name);
    }

    LOG_INFO("MapModifier::GetAllContracts: summary -> valid=" + std::to_string(validCount) +
        " nullContract=" + std::to_string(nullContract) +
        " nullInfo=" + std::to_string(nullInfo) +
        " nullMapName=" + std::to_string(nullMapName));

    return contracts;
}

void MapModifier::RefreshContracts()
{
    contracts.clear();
    selectedAutoVoteContract = 0;
    contractsAvailable = false;

    LOG_INFO("MapModifier::RefreshContracts: starting...");

    auto newContracts = GetAllContracts();
    if (newContracts.empty())
    {
        LOG_ERROR("MapModifier::RefreshContracts: failed to load contracts (empty list)");
        return;
    }

    contracts = std::move(newContracts);
    contractsAvailable = true;

    LOG_INFO("MapModifier::RefreshContracts: loaded " + std::to_string(contracts.size()) + " contracts");

    const auto selectedMap = CONFIG_STRING(GetConfigManager(), "AutoVoteMap");
    if (!selectedMap.empty())
    {
        bool found = false;
        for (int32_t i = 0; i < static_cast<int32_t>(contracts.size()); ++i)
        {
            if (contracts[i].contract_name == selectedMap)
            {
                selectedAutoVoteContract = i;
                found = true;
                LOG_INFO("MapModifier::RefreshContracts: restored selected map \"" + selectedMap + "\" at index " + std::to_string(i));
                break;
            }
        }
        if (!found)
        {
            LOG_ERROR("MapModifier::RefreshContracts: previously selected map \"" + selectedMap + "\" not found in current contracts");
        }
    }
    else
    {
        LOG_INFO("MapModifier::RefreshContracts: no previously selected map in config");
    }
}

void MapModifier::MapModifierMainAutoVote(SDK::LevelSelectionManager* levelSelectionManager)
{
    if (!CONFIG_BOOL(GetConfigManager(), "AutoSelectMap"))
        return;

    if (!levelSelectionManager)
        return;

    if (autoVoteDone.load())
        return;

    if (contracts.empty() || selectedAutoVoteContract < 0 ||
        selectedAutoVoteContract >= static_cast<int>(contracts.size()))
    {
        LOG_ERROR("AutoVote: contracts not ready");
        return;
    }

    auto* contract = contracts[selectedAutoVoteContract].contract_addr;
    if (!contract)
        return;

    if (levelSelectionManager->Fields.votedContract == contract)
    {
        autoVoteDone.store(true);
        return;
    }

    pendingAutoVote.store(true);
    LOG_INFO("AutoVote: pending vote set for \"" +
        contracts[selectedAutoVoteContract].contract_name + "\"");
}

*/