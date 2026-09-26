#include "mapmodifier.h"
using namespace PhasmoCheatV::Features::Map;

MapModifier::MapModifier() : FeatureCore(LANG("MapModifier_Header"), TYPE_MAP)
{
    DECLARE_CONFIG(GetConfigManager(), "CustomMaxLight", bool, false);
    DECLARE_CONFIG(GetConfigManager(), "MaxLight", int32_t, 10);
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