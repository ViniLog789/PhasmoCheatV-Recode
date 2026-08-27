#include "statspanel.h"

using namespace PhasmoCheatV::Features::Visuals;

StatsPanel::StatsPanel() : FeatureCore(LANG("LevelStatsPanel_Header"), TYPE_VISUALS) {}

void StatsPanel::OnRender()
{
    if (!IsActive()) return;

    const auto ls = Utils::GetLevelStats();

    if (!ls || !ls->Fields.boneRoom)
        return;

    if (!Utils::IsInGame()) 
        return;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16, 16));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8, 12));
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 6));

    constexpr ImGuiWindowFlags ghostWindowFlags =
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_AlwaysAutoResize |
        ImGuiWindowFlags_NoNav;

    ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiCond_Once);
    ImGui::Begin("###LevelStats", nullptr, ghostWindowFlags);

    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
    ImGui::TextColored(ImVec4(0.51f, 0.25f, 0.96f, 1.00f), "LEVEL STATS");
    ImGui::PopFont();
    ImGui::Separator();
    ImGui::Spacing();

    if (ImGui::BeginTable("LevelStatsPanel", 2,
        ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingFixedFit))
    {
        ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, 120);
        ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);

        auto DrawRow = [&](const char* label, const char* value) {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.9f, 1.0f), "%s", label);
            ImGui::TableSetColumnIndex(1);
            ImGui::TextWrapped("%s", value);
            };

        DrawRow("Bone room", Utils::UnityStrToSysStr(*ls->Fields.boneRoom).c_str());
        DrawRow("Bone collected", ls->Fields.collectedBone ? "Yes" : "No");
        DrawRow("Count hunts", std::to_string(ls->Fields.amountOfGhostHunts).c_str());
        DrawRow("Count interactions", std::to_string(ls->Fields.amountOfGhostInteractions).c_str());
        DrawRow("Count events", std::to_string(ls->Fields.amountOfGhostEvents).c_str());

        ImGui::EndTable();

		if (ImGui::Button("Collect Bone"))
            collectBone = true;
    }

    ImGui::End();
    ImGui::PopStyleVar(3);
}

void StatsPanel::OnMenuRender()
{
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 6));

    bool enabled = IsActive();
    if (ImGui::Checkbox(LANG("EnableLevelStatsPanel"), &enabled))
    {
        SET_CONFIG_VALUE(GetConfigManager(), "Enabled", bool, enabled);
        if (enabled) OnActivate();
        else OnDeactivate();
    }
    ImGui::PopStyleVar();
}

void StatsPanel::StatsPanelCollectBone()
{
    if (!collectBone) return;
    collectBone = false;
    if (!SDK::EvidenceController_sFields->instance)
    {
        NOTIFY_ERROR_QUICK(LANG("NeedToBeInGame"));
        return;
    }

    if (!SDK::PhotonNetwork_Get_IsMasterClient)
    {
        NOTIFY_ERROR_QUICK(LANG("NeedMustBeHost"));
        return;
    }

    if (!SDK::EvidenceController_sFields->instance->Fields.bone)
    {
        NOTIFY_ERROR_QUICK(LANG("BoneNotFound"));
        return;
	}

    SDK::DNAEvidence_GrabbedNetworked(SDK::EvidenceController_sFields->instance->Fields.bone, nullptr);
    NOTIFY_SUCCESS_QUICK(LANG("BoneCollected"));
}
