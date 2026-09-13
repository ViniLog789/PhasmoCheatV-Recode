#include "journalmod.h"

using namespace PhasmoCheatV::Features::Misc;

static std::unordered_map<std::string, int> ghostOrder = {
    {"Spirit", 0},
    {"Wraith", 1},
    {"Phantom", 2},
    {"Poltergeist", 3},
    {"Banshee", 4},
    {"Jinn", 5},
    {"Mare", 6},
    {"Revenant", 7},
    {"Shade", 8},
    {"Demon", 9},
    {"Yurei", 10},
    {"Oni", 11},
    {"Yokai", 12},
    {"Hantu", 13},
    {"Goryo", 14},
    {"Myling", 15},
    {"Onryo", 16},
    {"The Twins", 17},
    {"Raiju", 18},
    {"Obake", 19},
    {"The Mimic", 20},
    {"Moroi", 21},
    {"Deogen", 22},
    {"Thaye", 23},
    {"Dayan", 24},
    {"Gallu", 25},
    {"Obambo", 26}
};

JournalModifier::JournalModifier() : FeatureCore(LANG("JournalModifier_Header"), TYPE_MISC)
{
    DECLARE_CONFIG(GetConfigManager(), "OldGhostButtonPosition", bool, false);
    DECLARE_CONFIG(GetConfigManager(), "AutoSelectGhost", bool, false);
    DECLARE_CONFIG(GetConfigManager(), "CorrectGhostAlwaysFirst", bool, false);
}

void JournalModifier::OnActivate()
{
    lastUpdateTime = 0.0f;
}

void JournalModifier::OnDeactivate()
{
    lastUpdateTime = 0.0f;
}

void JournalModifier::OnRender()
{
    //! dont call SDK in render thread lol
}

void JournalModifier::OnMenuRender()
{
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 6));

    bool enabled = IsActive();
    if (ImGui::Checkbox(LANG("EnableJournalModifier"), &enabled))
    {
        SET_CONFIG_VALUE(GetConfigManager(), "Enabled", bool, enabled);
        if (enabled) OnActivate();
        else OnDeactivate();
    }

    if (enabled)
    {
        bool oldghostpos = CONFIG_BOOL(GetConfigManager(), "OldGhostButtonPosition");
        bool autoselectghost = CONFIG_BOOL(GetConfigManager(), "AutoSelectGhost");
        bool correctghostalwfirst = CONFIG_BOOL(GetConfigManager(), "CorrectGhostAlwaysFirst");

        if (ImGui::Checkbox(LANG("OldGhostButtonPosition"), &oldghostpos))
            SET_CONFIG_VALUE(GetConfigManager(), "OldGhostButtonPosition", bool, oldghostpos);

        if (ImGui::Checkbox(LANG("AutoSelectGhost"), &autoselectghost))
            SET_CONFIG_VALUE(GetConfigManager(), "AutoSelectGhost", bool, autoselectghost);

        if (ImGui::Checkbox(LANG("CorrectGhostAlwaysFirst"), &correctghostalwfirst))
            SET_CONFIG_VALUE(GetConfigManager(), "CorrectGhostAlwaysFirst", bool, correctghostalwfirst);
    }

    ImGui::PopStyleVar();
}

void JournalModifier::JournalModifierHandler()
{
    if (!IsActive() || !Utils::IsInGame())
        return;

    const float now = SDK::Time_Get_Time(nullptr);
    if (now <= 0.0f)
        return;

    if (lastUpdateTime > 0.0f && (now - lastUpdateTime) < 1.0f)
        return;

    lastUpdateTime = now;

    const int ghostType = static_cast<int>(Utils::GetGhostAI()->Fields.GhostInfo->Fields.GhostTraits.GhostType_);
    JournalModifierMain(ghostType);
}

void JournalModifier::JournalModifierMain(int GhostType)
{
    if (!IsActive()) return;

    bool oldghostpos = CONFIG_BOOL(GetConfigManager(), "OldGhostButtonPosition");
    bool autoselectghost = CONFIG_BOOL(GetConfigManager(), "AutoSelectGhost");
    bool correctghostalwfirst = CONFIG_BOOL(GetConfigManager(), "CorrectGhostAlwaysFirst");

    auto ghosts = Utils::get_AllGhostButtonsWithGO();

    if (ghosts.empty())
    {
        LOG_ERROR("Ghosts not found!");
        return;
    }

    std::string currentGhost = Utils::GhostEnumToStr(static_cast<SDK::GhostType>(GhostType));

    if (oldghostpos)
    {
        std::sort(ghosts.begin(), ghosts.end(),
            [](const auto& a, const auto& b)
            {
                const std::string& nameA = std::get<0>(a);
                const std::string& nameB = std::get<0>(b);

                int orderA = ghostOrder.contains(nameA) ? ghostOrder[nameA] : 999;
                int orderB = ghostOrder.contains(nameB) ? ghostOrder[nameB] : 999;

                return orderA < orderB;
            });

        int index = 0;

        for (const auto& [name, button, go] : ghosts)
        {
            if (!go) continue;

            auto transform = SDK::GameObject_get_transform(go, nullptr);
            if (!transform) continue;

            int currentIndex = SDK::Transform_GetSiblingIndex(transform, nullptr);
            if (currentIndex != index)
            {
                SDK::Transform_SetSiblingIndex(transform, index, nullptr);
            }

            index++;
        }
    }

    SDK::GhostButton* currentButton = nullptr;
    SDK::GameObject* targetGO = nullptr;

    for (const auto& [name, button, go] : ghosts)
    {
        if (name == currentGhost)
        {
            currentButton = button;
            targetGO = go;
            break;
        }
    }

    if (autoselectghost && currentButton)
    {
        auto& state = currentButton->Fields.threeStateButtonFields.currentState;

        if (state != SDK::ThreeStateButtonState::Ticked)
        {
            auto* func = SDK::Get_GhostButton_OnClick();
            func(currentButton, nullptr);
        }
    }

    if (correctghostalwfirst && targetGO)
    {
        auto transform = SDK::GameObject_get_transform(targetGO, nullptr);
        if (!transform)
        {
            LOG_ERROR("Transform is null!");
        }
        else
        {
            int currentIndex = SDK::Transform_GetSiblingIndex(transform, nullptr);
            if (currentIndex != 0)
            {
                SDK::Transform_SetSiblingIndex(transform, 0, nullptr);
            }
        }
    }
}
