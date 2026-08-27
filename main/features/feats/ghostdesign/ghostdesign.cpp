#include "ghostdesign.h"

using namespace PhasmoCheatV::Features::Ghost;

GhostDesigner::GhostDesigner() : FeatureCore(LANG("GhostDesigner(HOST)_Header"), TYPE_GHOST)
{
    DECLARE_CONFIG(GetConfigManager(), "GhostAge", int, 0);
    DECLARE_CONFIG(GetConfigManager(), "GhostType", int, static_cast<int>(SDK::GhostType::Spirit));
    DECLARE_CONFIG(GetConfigManager(), "IsShy", bool, false);
    DECLARE_CONFIG(GetConfigManager(), "EvidenceMode", int, 0);
    DECLARE_CONFIG(GetConfigManager(), "EvidenceCount", int, 3);

    for (int i = 0; i < 7; i++) {
        DECLARE_CONFIG(GetConfigManager(), "Evidence" + std::to_string(i), int, i + 1);
    }
}

void GhostDesigner::OnMenuRender()
{
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 6));

    auto MakeLabel = [](const char* code, const char* id) -> std::string
        {
            return std::string(LANG(code)) + "##" + id;
        };

    bool enabled = IsActive();
    if (ImGui::Checkbox(LANG("EnableGhostDesigner"), &enabled))
    {
        SET_CONFIG_VALUE(GetConfigManager(), "Enabled", bool, enabled);
        enabled ? OnActivate() : OnDeactivate();
    }

    if (!enabled)
    {
        ImGui::PopStyleVar();
        return;
    }

    int ghostAge = CONFIG_INT(GetConfigManager(), "GhostAge");
    if (ImGui::SliderInt(
        MakeLabel("GhostAge", "age").c_str(),
        &ghostAge, -1000, 1000))
    {
        SET_CONFIG_VALUE(GetConfigManager(), "GhostAge", int, ghostAge);
    }

    const char* ghostTypes[] = {
        LANG("GhostType_Spirit"), LANG("GhostType_Wraith"), LANG("GhostType_Phantom"), LANG("GhostType_Poltergeist"), LANG("GhostType_Banshee"),
        LANG("GhostType_Jinn"), LANG("GhostType_Mare"), LANG("GhostType_Revenant"), LANG("GhostType_Shade"), LANG("GhostType_Demon"),
        LANG("GhostType_Yurei"), LANG("GhostType_Oni"), LANG("GhostType_Hantu"), LANG("GhostType_Yokai"), LANG("GhostType_Goryo"),
        LANG("GhostType_Myling"), LANG("GhostType_Onryo"), LANG("GhostType_TheTwins"), LANG("GhostType_Raiju"), LANG("GhostType_Obake"),
        LANG("GhostType_Mimic"), LANG("GhostType_Moroi"), LANG("GhostType_Deogen"), LANG("GhostType_Thaye"), LANG("GhostType_Default"),
		LANG("GhostType_Gallu"), LANG("GhostType_Dayan"), LANG("GhostType_Obambo"), LANG("GhostType_Kormos"), LANG("GhostType_Aswang"), LANG("GhostType_Deildegast")
    };

    int ghostType = CONFIG_INT(GetConfigManager(), "GhostType");
    if (ImGui::Combo(
        MakeLabel("GhostType", "type").c_str(),
        &ghostType,
        ghostTypes,
        IM_ARRAYSIZE(ghostTypes)))
    {
        SET_CONFIG_VALUE(GetConfigManager(), "GhostType", int, ghostType);
    }

    bool isShy = CONFIG_BOOL(GetConfigManager(), "IsShy");
    if (ImGui::Checkbox(
        MakeLabel("IsShy", "shy").c_str(),
        &isShy))
    {
        SET_CONFIG_VALUE(GetConfigManager(), "IsShy", bool, isShy);
    }

    ImGui::Separator();
    ImGui::TextUnformatted(LANG("EvidenceSettings"));

    const char* evidenceModes[] = {
        LANG("EvidenceModeRandom"),
        LANG("EvidenceModeSelected")
    };

    int evidenceMode = CONFIG_INT(GetConfigManager(), "EvidenceMode");
    if (ImGui::Combo(
        MakeLabel("EvidenceMode", "mode").c_str(),
        &evidenceMode,
        evidenceModes,
        IM_ARRAYSIZE(evidenceModes)))
    {
        SET_CONFIG_VALUE(GetConfigManager(), "EvidenceMode", int, evidenceMode);
    }

    int evidenceCount = CONFIG_INT(GetConfigManager(), "EvidenceCount");
    if (ImGui::SliderInt(
        MakeLabel("EvidenceCount", "count").c_str(),
        &evidenceCount, 0, 7))
    {
        SET_CONFIG_VALUE(GetConfigManager(), "EvidenceCount", int, evidenceCount);
    }

    if (evidenceMode == 1)
    {
        const char* evidenceNames[] = {
            "None",
            "EMF Level 5",
            "Spirit Box",
            "Fingerprints",
            "Ghost Orb",
            "Ghost Writing",
            "Freezing Temps",
            "D.O.T.S"
        };

        for (int i = 0; i < evidenceCount; i++)
        {
            std::string configKey = "Evidence" + std::to_string(i);
            int evidenceValue = CONFIG_INT(GetConfigManager(), configKey);

            std::string label =
                std::string(LANG("Evidence")) + " " +
                std::to_string(i + 1) + "##evidence_" +
                std::to_string(i);

            if (ImGui::Combo(
                label.c_str(),
                &evidenceValue,
                evidenceNames,
                IM_ARRAYSIZE(evidenceNames)))
            {
                SET_CONFIG_VALUE(GetConfigManager(), configKey, int, evidenceValue);
            }
        }
    }

    ImGui::PopStyleVar();
}

void GhostDesigner::ApplyGhostSettings(int& ghostType, int& ghostAge, bool& isShy)
{
    if (!CONFIG_BOOL(GetConfigManager(), "Enabled"))
        return;

    ghostType = CONFIG_INT(GetConfigManager(), "GhostType");
    ghostAge = CONFIG_INT(GetConfigManager(), "GhostAge");
    isShy = CONFIG_BOOL(GetConfigManager(), "IsShy");
}

void GhostDesigner::ApplyGhostEvidence(int* evidence)
{
    if (!evidence)
        return;

    if (!CONFIG_BOOL(GetConfigManager(), "Enabled"))
        return;

    int mode = CONFIG_INT(GetConfigManager(), "EvidenceMode");
    int evidenceCount = CONFIG_INT(GetConfigManager(), "EvidenceCount");

    int count = *(int*)((uintptr_t)evidence + 0x18);
    int* evidenceData = (int*)((uintptr_t)evidence + 0x20);

    std::vector<int> newEvidence;

    if (mode == 0)
    {
        std::vector<int> pool = { 1,2,3,4,5,6,7 };
        std::mt19937 gen(std::random_device{}());
        std::shuffle(pool.begin(), pool.end(), gen);
        evidenceCount = std::clamp(evidenceCount, 0, (int)pool.size());
        newEvidence.assign(pool.begin(), pool.begin() + evidenceCount);
    }
    else
    {
        for (int i = 0; i < evidenceCount; i++)
            newEvidence.push_back(CONFIG_INT(GetConfigManager(), "Evidence" + std::to_string(i)));
    }

    *(int*)((uintptr_t)evidence + 0x18) = (int)newEvidence.size();
    for (size_t i = 0; i < newEvidence.size(); i++)
        evidenceData[i] = newEvidence[i];
}