#include "difficultymodifier.h"

using namespace PhasmoCheatV::Features::Difficulty;

DifficultyModifier::DifficultyModifier() : FeatureCore(LANG("DifficultyModifier_Header"), TYPE_DIFFICULTY)
{
    DECLARE_CONFIG(GetConfigManager(), "RequiredLevel", int, 0);
    DECLARE_CONFIG(GetConfigManager(), "SanityPillRestoration", int, 0);
    DECLARE_CONFIG(GetConfigManager(), "StartingSanity", int, 0);
    DECLARE_CONFIG(GetConfigManager(), "SanityDrain", float, 0.0f);
    DECLARE_CONFIG(GetConfigManager(), "Sprinting", int, 0);
    DECLARE_CONFIG(GetConfigManager(), "Flashlights", bool, false);
    DECLARE_CONFIG(GetConfigManager(), "LoseItemsOnDeath", bool, false);
    DECLARE_CONFIG(GetConfigManager(), "PlayerSpeed", float, 0.0f);
    DECLARE_CONFIG(GetConfigManager(), "EvidenceGiven", int, 0);
    DECLARE_CONFIG(GetConfigManager(), "ChangingFavouriteRoom", int, 0);
    DECLARE_CONFIG(GetConfigManager(), "InteractionAmount", int, 0);
    DECLARE_CONFIG(GetConfigManager(), "EventFrequency", int, 0);
    DECLARE_CONFIG(GetConfigManager(), "HuntDuration", int, 0);
    DECLARE_CONFIG(GetConfigManager(), "GracePeriod", int, 0);
    DECLARE_CONFIG(GetConfigManager(), "FingerprintChance", float, 0.0f);
    DECLARE_CONFIG(GetConfigManager(), "FingerprintDuration", int, 0);
    DECLARE_CONFIG(GetConfigManager(), "FriendlyGhost", bool, false);
    DECLARE_CONFIG(GetConfigManager(), "KillsExtendHunt", int, 0);
    DECLARE_CONFIG(GetConfigManager(), "RoamingFrequency", int, 0);
    DECLARE_CONFIG(GetConfigManager(), "GhostSpeed", float, 0.0f);
    DECLARE_CONFIG(GetConfigManager(), "SetupTime", int, 0);
    DECLARE_CONFIG(GetConfigManager(), "SelectedWeather", int, 0);
    DECLARE_CONFIG(GetConfigManager(), "DoorsOpen", int, 0);
    DECLARE_CONFIG(GetConfigManager(), "HidingPlaces", int, 0);
    DECLARE_CONFIG(GetConfigManager(), "SanityMonitor", bool, false);
    DECLARE_CONFIG(GetConfigManager(), "ActivityMonitor", bool, false);
    DECLARE_CONFIG(GetConfigManager(), "FuseBoxVisible", bool, false);
    DECLARE_CONFIG(GetConfigManager(), "FuseBoxStartPower", int, 0);
}

void DifficultyModifier::OnMenuRender()
{
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 6));

    bool enabled = IsActive();
    if (ImGui::Checkbox(LANG("EnableDifficultyModifier"), &enabled))
    {
        SET_CONFIG_VALUE(GetConfigManager(), "Enabled", bool, enabled);
        if (enabled) OnActivate();
        else OnDeactivate();
    }

    if (!enabled)
    {
        ImGui::PopStyleVar();
        return;
    }

    static int selectedSetting = 0;
    ImGui::Text(LANG("AddSettingToModify"));

    std::vector<const char*> settingNames;
    for (const auto& setting : m_availableSettings)
    {
        settingNames.push_back(setting.first.c_str());
    }

    if (ImGui::Combo("##SettingSelect", &selectedSetting, settingNames.data(), static_cast<int>(settingNames.size())))
    {

    }

    ImGui::SameLine();
    if (ImGui::Button(LANG("AddSetting")))
    {
        AddSettingToModify(selectedSetting);
    }

    ImGui::Separator();

    ImGui::Text(LANG("SelectedSettings"));

    for (size_t i = 0; i < m_selectedSettings.size(); i++)
    {
        int settingIndex = m_selectedSettings[i];
        RenderSettingEditor(i);
    }

    if (!m_selectedSettings.empty())
    {
        if (ImGui::Button(LANG("ApplyChanges")))
        {
            DifficultyModifierApply();
        }

        ImGui::SameLine();
        if (ImGui::Button(LANG("ClearAll")))
        {
            m_selectedSettings.clear();
        }
    }

    ImGui::PopStyleVar();
}

void DifficultyModifier::AddSettingToModify(int settingIndex)
{

    for (int existing : m_selectedSettings)
    {
        if (existing == settingIndex)
            return;
    }

    m_selectedSettings.push_back(settingIndex);
}

void DifficultyModifier::RemoveSetting(int index)
{
    if (index >= 0 && index < m_selectedSettings.size())
    {
        m_selectedSettings.erase(m_selectedSettings.begin() + index);
    }
}

void DifficultyModifier::RenderSettingEditor(int index)
{
    if (index < 0 || index >= m_selectedSettings.size())
        return;

    int settingIndex = m_selectedSettings[index];
    const std::string& settingName = m_availableSettings[settingIndex].first;

    ImGui::PushID(index);

    ImGui::BeginGroup();
    ImGui::Text("%s", LANG(settingName.c_str()));
    ImGui::SameLine();
    if (ImGui::Button("X"))
    {
        RemoveSetting(index);
        ImGui::PopID();
        return;
    }
    ImGui::EndGroup();

    switch (settingIndex)
    {

    case 0: case 1: case 2: case 13: case 15: case 20:
    {
        int value = CONFIG_INT(GetConfigManager(), GetConfigKey(settingIndex));
        if (ImGui::InputInt("##Value", &value))
        {
            SET_CONFIG_VALUE(GetConfigManager(), GetConfigKey(settingIndex), int, value);
        }
        break;
    }

    case 3: case 14: case 19:
    {
        float value = CONFIG_FLOAT(GetConfigManager(), GetConfigKey(settingIndex));
        if (ImGui::InputFloat("##Value", &value, 0.1f, 1.0f, "%.1f"))
        {
            SET_CONFIG_VALUE(GetConfigManager(), GetConfigKey(settingIndex), float, value);
        }
        break;
    }

    case 5: case 6: case 16: case 24: case 25: case 26:
    {
        bool value = CONFIG_BOOL(GetConfigManager(), GetConfigKey(settingIndex));
        if (ImGui::Checkbox("##Value", &value))
        {
            SET_CONFIG_VALUE(GetConfigManager(), GetConfigKey(settingIndex), bool, value);
        }
        break;
    }

    case 4:
    {
        int value = CONFIG_INT(GetConfigManager(), GetConfigKey(settingIndex));
        const char* modes[] = { LANG("Off"), LANG("On"), LANG("Infinite") };
        if (ImGui::Combo("##Value", &value, modes, IM_ARRAYSIZE(modes)))
        {
            SET_CONFIG_VALUE(GetConfigManager(), GetConfigKey(settingIndex), int, value);
        }
        break;
    }

    case 8:
    {
        int value = CONFIG_INT(GetConfigManager(), GetConfigKey(settingIndex));
        if (ImGui::SliderInt("##Value", &value, 0, 3))
        {
            SET_CONFIG_VALUE(GetConfigManager(), GetConfigKey(settingIndex), int, value);
        }
        break;
    }

    case 21:
    {
        int value = CONFIG_INT(GetConfigManager(), GetConfigKey(settingIndex));
        const char* weather[] = {
            LANG("Random"),
            LANG("LightRain"),
            LANG("HeavyRain"),
            LANG("Snow"),
            LANG("Wind"),
            LANG("Clear"),
            LANG("Fog"),
            LANG("Sunrise"),
            LANG("BloodMoon")
        };
        if (ImGui::Combo("##Value", &value, weather, IM_ARRAYSIZE(weather)))
        {
            SET_CONFIG_VALUE(GetConfigManager(), GetConfigKey(settingIndex), int, value);
        }
        break;
    }

    case 27:
    {
        int value = CONFIG_INT(GetConfigManager(), GetConfigKey(settingIndex));
        const char* power[] = { LANG("Broken"), LANG("Off"), LANG("On") };
        if (ImGui::Combo("##Value", &value, power, IM_ARRAYSIZE(power)))
        {
            SET_CONFIG_VALUE(GetConfigManager(), GetConfigKey(settingIndex), int, value);
        }
        break;
    }

    case 9: case 10: case 11: case 12: case 17: case 18: case 22: case 23:
    {
        int value = CONFIG_INT(GetConfigManager(), GetConfigKey(settingIndex));
        const char* difficulty[] = {
            LANG("None"),
            LANG("Low"),
            LANG("Medium"),
            LANG("High"),
            LANG("VeryHigh")
        };
        if (ImGui::Combo("##Value", &value, difficulty, IM_ARRAYSIZE(difficulty)))
        {
            SET_CONFIG_VALUE(GetConfigManager(), GetConfigKey(settingIndex), int, value);
        }
        break;
    }

    case 7:
    {
        float value = CONFIG_FLOAT(GetConfigManager(), GetConfigKey(settingIndex));
        if (ImGui::InputFloat("##Value", &value, 0.1f, 1.0f, "%.1f"))
        {
            SET_CONFIG_VALUE(GetConfigManager(), GetConfigKey(settingIndex), float, value);
        }
        break;
    }
    }

    ImGui::PopID();
    ImGui::Separator();
}

const char* DifficultyModifier::GetConfigKey(int settingIndex) const
{
    static const std::vector<const char*> configKeys = {
        "RequiredLevel", "SanityPillRestoration", "StartingSanity", "SanityDrain", "Sprinting",
        "Flashlights", "LoseItemsOnDeath", "PlayerSpeed", "EvidenceGiven", "ChangingFavouriteRoom",
        "InteractionAmount", "EventFrequency", "HuntDuration", "GracePeriod", "FingerprintChance",
        "FingerprintDuration", "FriendlyGhost", "KillsExtendHunt", "RoamingFrequency", "GhostSpeed",
        "SetupTime", "SelectedWeather", "DoorsOpen", "HidingPlaces", "SanityMonitor",
        "ActivityMonitor", "FuseBoxVisible", "FuseBoxStartPower"
    };

    return configKeys[settingIndex];
}

void DifficultyModifier::DifficultyModifierApply()
{
    try
    {
        SDK::LevelValues* instance = Utils::GetLevelValues();
        if (!instance)
        {
            NOTIFY_ERROR_QUICK(LANG("LevelValuesNotFound"));
            return;
        }

        if (!instance->Fields.currentDifficulty)
        {
            NOTIFY_ERROR_QUICK(LANG("NoCurrentDifficulty"));
            return;
        }

        auto& diff = instance->Fields.currentDifficulty->Fields;

        for (int settingIndex : m_selectedSettings)
        {
            switch (settingIndex)
            {
            case 0: diff.requiredLevel = CONFIG_INT(GetConfigManager(), "RequiredLevel"); break;
            case 1: diff.sanityPillRestorationAmount = CONFIG_INT(GetConfigManager(), "SanityPillRestoration"); break;
            case 2: diff.startingSanity = CONFIG_INT(GetConfigManager(), "StartingSanity"); break;
            case 3: diff.sanityDrain = CONFIG_FLOAT(GetConfigManager(), "SanityDrain"); break;
            case 4: diff.sprinting = CONFIG_INT(GetConfigManager(), "Sprinting"); break;
            case 5: diff.flashlights = CONFIG_BOOL(GetConfigManager(), "Flashlights"); break;
            case 6: diff.loseItemsOnDeath = CONFIG_BOOL(GetConfigManager(), "LoseItemsOnDeath"); break;
            case 7: diff.playerSpeed = CONFIG_FLOAT(GetConfigManager(), "PlayerSpeed"); break;
            case 8: diff.evidenceGiven = CONFIG_INT(GetConfigManager(), "EvidenceGiven"); break;
            case 9: diff.changingFav = static_cast<SDK::DifficultyValueType>(CONFIG_INT(GetConfigManager(), "ChangingFavouriteRoom")); break;
            case 10: diff.interaction = static_cast<SDK::DifficultyValueType>(CONFIG_INT(GetConfigManager(), "InteractionAmount")); break;
            case 11: diff.eventFreq = static_cast<SDK::DifficultyValueType>(CONFIG_INT(GetConfigManager(), "EventFrequency")); break;
            case 12: diff.huntDuration = static_cast<SDK::DifficultyValueType>(CONFIG_INT(GetConfigManager(), "HuntDuration")); break;
            case 13: diff.gracePeriod = CONFIG_INT(GetConfigManager(), "GracePeriod"); break;
            case 14: diff.fingerprintChance = CONFIG_FLOAT(GetConfigManager(), "FingerprintChance"); break;
            case 15: diff.fingerprintDuration = CONFIG_INT(GetConfigManager(), "FingerprintDuration"); break;
            case 16: diff.friendlyGhost = CONFIG_BOOL(GetConfigManager(), "FriendlyGhost"); break;
            case 17: diff.killsExtendHuntDuration = static_cast<SDK::DifficultyValueType>(CONFIG_INT(GetConfigManager(), "KillsExtendHunt")); break;
            case 18: diff.roamingFrequency = static_cast<SDK::DifficultyValueType>(CONFIG_INT(GetConfigManager(), "RoamingFrequency")); break;
            case 19: diff.ghostSpeed = CONFIG_FLOAT(GetConfigManager(), "GhostSpeed"); break;
            case 20: diff.setupTime = CONFIG_INT(GetConfigManager(), "SetupTime"); break;
            case 21:
                diff.selectedWeather = static_cast<SDK::DifficultyWeatherValueType>(CONFIG_INT(GetConfigManager(), "SelectedWeather"));
                diff.actualWeather = static_cast<SDK::DifficultyWeatherValueType>(CONFIG_INT(GetConfigManager(), "SelectedWeather"));
                break;
            case 22: diff.amountDoorsOpen = static_cast<SDK::DifficultyValueType>(CONFIG_INT(GetConfigManager(), "DoorsOpen")); break;
            case 23: diff.amountHidingPlaces = static_cast<SDK::DifficultyValueType>(CONFIG_INT(GetConfigManager(), "HidingPlaces")); break;
            case 24: diff.sanityMonitor = CONFIG_BOOL(GetConfigManager(), "SanityMonitor"); break;
            case 25: diff.activityMonitor = CONFIG_BOOL(GetConfigManager(), "ActivityMonitor"); break;
            case 26: diff.fuseBoxVisible = CONFIG_BOOL(GetConfigManager(), "FuseBoxVisible"); break;
            case 27: diff.fuseBoxStartPower = static_cast<SDK::DifficultyFuseBoxValueType>(CONFIG_INT(GetConfigManager(), "FuseBoxStartPower")); break;
            }
        }

        NOTIFY_SUCCESS_QUICK(LANG("DifficultySettingsApplied"));
    }
    catch (const std::exception& e)
    {
        NOTIFY_ERROR_QUICK(LANG("FailedApplyDifficultySettings"));
    }
}