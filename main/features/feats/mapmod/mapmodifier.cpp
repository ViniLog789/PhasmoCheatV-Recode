#include "mapmodifier.h"
using namespace PhasmoCheatV::Features::Map;

constexpr int32_t mapIds[] = {
    -1,  // Random
    4,   // Tanglewood Drive
    6,   // Edgefield Road
    7,   // Ridgeview Court
    42,   // Nell's Diner
    10,  // Grafton Farmhouse
    12,  // Willow Street
    15,  // Point Hope
    8,   // Brownstone High School
    9,   // Bleasdale Farmhouse
    5,   // Sunny Meadows
    5,   // Sunny Meadows restricted
    11,  // Prison
    13,  // Maple Lodge Campsite
    14   // Camp Woodwind
};

const char* mapItems[] = {
    "Random",
    "Tanglewood Drive",
    "Edgefield Road",
    "Ridgeview Court",
    "Nell's Diner",
    "Grafton Farmhouse",
    "Willow Street",
    "Point Hope",
    "Brownstone High School",
    "Bleasdale Farmhouse",
    "Sunny Meadows",
    "Sunny Meadows Restricted",
    "Prison",
    "Maple Lodge Campsite",
    "Camp Woodwind"
};

MapModifier::MapModifier() : FeatureCore(LANG("MapModifier_Header"), TYPE_MAP)
{
    DECLARE_CONFIG(GetConfigManager(), "CustomMaxLight", bool, false);
    DECLARE_CONFIG(GetConfigManager(), "MaxLight", int32_t, 10);
    DECLARE_CONFIG(GetConfigManager(), "AutoSelectMap", bool, false);
    DECLARE_CONFIG(GetConfigManager(), "AutoVoteMapId", int32_t, -1);
}

void MapModifier::OnMenuRender()
{
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 6));

    bool AutoSelectMap = CONFIG_BOOL(GetConfigManager(), "AutoSelectMap");
    int32_t AutoVoteMapId = CONFIG_INT(GetConfigManager(), "AutoVoteMapId");

    SDK::Map* mapInstance = Utils::GetMap();
    if (mapInstance && mapInstance->Fields.mapName)
    {
        std::string CurrentMapName = Utils::UnityStrToSysStr(*mapInstance->Fields.mapName);
        ImGui::Text(LANG("CurrentMap"), CurrentMapName.c_str());
    }

    if (ImGui::Checkbox(LANG("AutoSelectMap"), &AutoSelectMap))
    {
        SET_CONFIG_VALUE(GetConfigManager(), "AutoSelectMap", bool, AutoSelectMap);
    }

    if (AutoSelectMap)
    {
        int currentItem = 0;
        for (int i = 0; i < IM_ARRAYSIZE(mapIds); i++)
        {
            if (mapIds[i] == AutoVoteMapId)
            {
                currentItem = i;
                break;
            }
        }

        if (ImGui::Combo(LANG("SelectMap"), &currentItem, mapItems, IM_ARRAYSIZE(mapItems)))
        {
            int32_t newMapId = mapIds[currentItem];
            SET_CONFIG_VALUE(GetConfigManager(), "AutoVoteMapId", int32_t, newMapId);
            selectedAutoVoteMap = static_cast<MapIdsAutoVote>(newMapId);
        }
    }

    bool enabled = IsActive();
    bool CustomMaxLight = CONFIG_BOOL(GetConfigManager(), "CustomMaxLight");
    int32_t MaxLight = CONFIG_INT(GetConfigManager(), "MaxLight");

    if (ImGui::Checkbox(LANG("EnableMapModifier"), &enabled))
    {
        SET_CONFIG_VALUE(GetConfigManager(), "Enabled", bool, enabled);
        if (enabled) OnActivate();
        else OnDeactivate();
    }

    if (enabled)
    {
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
    }

    ImGui::PopStyleVar();
}

void MapModifier::MapModifierMain()
{
    if (IsActive() && CONFIG_BOOL(GetConfigManager(), "CustomMaxLight") && SDK::LevelController_sFields->instance && SDK::LevelController_sFields->instance->Fields.fuseBox)
    {
        SDK::LevelController_sFields->instance->Fields.fuseBox->Fields.maxLights = CONFIG_INT(GetConfigManager(), "MaxLight");
    }
    
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

        if (weatherProfile->Fields.weatherType != SDK::WeatherType::heavyRain) // NEVER REMOVE THIS IF LOOP
        {
            NOTIFY_ERROR_QUICK(LANG("WeatherShouldBeHeavyRain"));
            return;
        }

        if (!SDK::PhotonNetwork_Get_IsMasterClient(nullptr) || !SDK::PhotonNetwork_Get_OfflineMode(nullptr)) // NEVER REMOVE THIS IF LOOP
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

std::atomic<bool> autoVoteRunning{ false };

void MapModifier::MapModifierMainAutoVote(SDK::LevelSelectionManager* levelSelectionManager)
{
    if (autoVoteRunning.load())
        return;

    if (!CONFIG_BOOL(GetConfigManager(), "AutoSelectMap"))
        return;

    autoVoteRunning.store(true);
    std::thread([this, levelSelectionManager]() {
        auto start = std::chrono::steady_clock::now();

        while (true)
        {
            auto now = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count();

            if (elapsed >= 5000)
            {
                if (levelSelectionManager)
                {
                    int32_t mapId = CONFIG_INT(GetConfigManager(), "AutoVoteMapId");
                    SDK::LevelSelectionManager_VoteMap(levelSelectionManager, mapId, nullptr);
                }
                break;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        autoVoteRunning.store(false);
        }).detach();
}
