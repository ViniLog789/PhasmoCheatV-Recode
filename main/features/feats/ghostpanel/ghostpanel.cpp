#include "ghostpanel.h"

using namespace PhasmoCheatV::Features::Visuals;

GhostPanel::GhostPanel() : FeatureCore(LANG("GhostPanel_Header"), TYPE_VISUALS)
{
    DECLARE_CONFIG(GetConfigManager(), "BansheeTargetSetting", bool, false);
    DECLARE_CONFIG(GetConfigManager(), "IsHideSettings", bool, false);
    DECLARE_CONFIG(GetConfigManager(), "HideName", bool, false);
    DECLARE_CONFIG(GetConfigManager(), "HideType", bool, false);
    DECLARE_CONFIG(GetConfigManager(), "HideAge", bool, false);
    DECLARE_CONFIG(GetConfigManager(), "HideState", bool, false);
    DECLARE_CONFIG(GetConfigManager(), "HideEvidence", bool, false);
    DECLARE_CONFIG(GetConfigManager(), "HideRoom", bool, false);
    DECLARE_CONFIG(GetConfigManager(), "HideLocation", bool, false);
    DECLARE_CONFIG(GetConfigManager(), "HideMimicType", bool, false);
    DECLARE_CONFIG(GetConfigManager(), "HideBansheeTarget", bool, false);
    DECLARE_CONFIG(GetConfigManager(), "HideCurrentSpeed", bool, false);
    DECLARE_CONFIG(GetConfigManager(), "HideHuntDuration", bool, false);
    DECLARE_CONFIG(GetConfigManager(), "HideGhostSmudged", bool, false);
    DECLARE_CONFIG(GetConfigManager(), "HideGhostAfterHunting", bool, false);
    DECLARE_CONFIG(GetConfigManager(), "RowOrder", std::string, "Name;Type;Age;State;Mimic;Banshee;Evidence;Room;Location;GhostCurrentSpeed;HuntDuration;GhostSmudged;GhostAfterHunting;");
}

void GhostPanel::LoadRowOrder()
{
    m_rowOrder.clear();
    const std::string raw = CONFIG_STRING(GetConfigManager(), "RowOrder");

    size_t start = 0;
    while (start < raw.size())
    {
        size_t sep = raw.find(';', start);
        if (sep == std::string::npos) sep = raw.size();
        m_rowOrder.push_back(raw.substr(start, sep - start));
        start = sep + 1;
    }

    if (m_rowOrder.empty())
        m_rowOrder = { "Name", "Type", "Age", "State", "Mimic", "Banshee", "Evidence", "Room", "Location", "GhostCurrentSpeed", "HuntDuration", "GhostSmudged", "GhostAfterHunting" };
}

void GhostPanel::SaveRowOrder()
{
    std::string raw;
    for (size_t i = 0; i < m_rowOrder.size(); ++i)
    {
        if (i > 0) raw += ";";
        raw += m_rowOrder[i];
    }
    SET_CONFIG_VALUE(GetConfigManager(), "RowOrder", std::string, raw);
}

void GhostPanel::DrawReorderableRows(std::vector<RowDef>& rows)
{
    std::unordered_map<std::string, RowDef*> byId;
    for (auto& r : rows) byId[r.id] = &r;

    std::vector<std::string> activeOrder;
    for (auto& id : m_rowOrder)
        if (byId.count(id) && !byId[id]->value.empty())
            activeOrder.push_back(id);

    for (auto& r : rows)
        if (std::find(activeOrder.begin(), activeOrder.end(), r.id) == activeOrder.end() && !r.value.empty())
            activeOrder.push_back(r.id);

    bool orderChanged = false;

    for (int i = 0; i < (int)activeOrder.size(); ++i)
    {
        RowDef* row = byId[activeOrder[i]];
        ImGui::PushID(row->id.c_str());

        ImGui::Selectable("##drag_handle", false, ImGuiSelectableFlags_None, ImVec2(0, 0));

        // source
        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
        {
            ImGui::SetDragDropPayload("GHOST_ROW", &i, sizeof(int));
            ImGui::Text("%s", row->label.c_str());
            ImGui::EndDragDropSource();
        }

        // target
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("GHOST_ROW"))
            {
                int srcIndex = *(const int*)payload->Data;
                if (srcIndex != i)
                {
                    std::string moved = activeOrder[srcIndex];
                    activeOrder.erase(activeOrder.begin() + srcIndex);
                    activeOrder.insert(activeOrder.begin() + i, moved);
                    orderChanged = true;
                }
            }
            ImGui::EndDragDropTarget();
        }

        ImGui::SameLine();
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.9f, 1.0f), "%s", row->label.c_str());
        ImGui::SameLine(140.0f);

        if (row->hidden && CONFIG_BOOL(GetConfigManager(), "IsHideSettings"))
            DrawHiddenValue();
        else
            ImGui::TextWrapped("%s", row->value.c_str());

        ImGui::PopID();
    }

    if (orderChanged)
    {
        m_rowOrder = activeOrder;
        SaveRowOrder();
    }
}

void GhostPanel::DrawHiddenValue(float width, float height)
{
    ImVec2 pos = ImGui::GetCursorScreenPos();
    ImDrawList* draw = ImGui::GetWindowDrawList();

    draw->AddRectFilled(
        pos,
        ImVec2(pos.x + width, pos.y + height),
        IM_COL32(0, 0, 0, 255),
        4.0f
    );

    ImGui::Dummy(ImVec2(width, height));
}

void GhostPanel::OnRender()
{
    if (!IsActive()) return;

    if (!Utils::IsInGame())
        return;

    if (!m_rowOrderLoaded)
    {
        LoadRowOrder();
        m_rowOrderLoaded = true;
    }

    const auto& ghostInfo = Utils::GetGhostAI()->Fields.GhostInfo;
    const auto& ghostTraits = ghostInfo->Fields.GhostTraits;

    if (!ghostTraits.Name)
        return;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16, 16));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8, 12));
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 6));

    constexpr ImGuiWindowFlags ghostWindowFlags =
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_AlwaysAutoResize |
        ImGuiWindowFlags_NoNav;

    ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiCond_Once);
    ImGui::Begin("###GhostWindow", nullptr, ghostWindowFlags);

    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
    ImGui::TextColored(ImVec4(0.51f, 0.25f, 0.96f, 1.00f), "GHOST PROFILE");
    ImGui::PopFont();
    ImGui::Separator();
    ImGui::Spacing();

    std::vector<RowDef> rows;

    rows.push_back({ "Name", LANG("GhostName"), Utils::UnityStrToSysStr(*ghostTraits.Name), CONFIG_BOOL(GetConfigManager(), "HideName") });
    rows.push_back({ "Type", LANG("GhostType"), Utils::GhostEnumToStrLocalized(ghostTraits.GhostType_), CONFIG_BOOL(GetConfigManager(), "HideType") });
    rows.push_back({ "Age", LANG("GhostAge"), std::to_string(ghostTraits.GhostAge), CONFIG_BOOL(GetConfigManager(), "HideAge") });
    rows.push_back({ "State", LANG("GhostState"), Utils::GhostEnumToStr(Utils::GetGhostAI()->Fields.currentState), CONFIG_BOOL(GetConfigManager(), "HideState") });

    if (ghostTraits.GhostType_ == SDK::GhostType::Mimic)
        rows.push_back({ "Mimic", LANG("MimicType"), Utils::GhostEnumToStr(ghostTraits.MimicType), CONFIG_BOOL(GetConfigManager(), "HideMimicType") });

    if (CONFIG_BOOL(GetConfigManager(), "BansheeTargetSetting") && ghostTraits.GhostType_ == SDK::GhostType::Banshee)
        if (const auto& bansheeTarget = Utils::GetGhostAI()->Fields.BansheeTarget)
            rows.push_back({ "Banshee", LANG("BansheeTarget"), Utils::GetPlayerName(bansheeTarget), CONFIG_BOOL(GetConfigManager(), "HideBansheeTarget") });

    if (const auto& evidence = GetGhostEvidenceString(); !evidence.empty())
        rows.push_back({ "Evidence", LANG("Evidence"), evidence, CONFIG_BOOL(GetConfigManager(), "HideEvidence") });

    if (const auto& levelRoom = ghostInfo->Fields.favouriteRoom; levelRoom && levelRoom->Fields.RoomName)
        rows.push_back({ "Room", LANG("FavoriteRoom"), Utils::UnityStrToSysStr(*levelRoom->Fields.RoomName), CONFIG_BOOL(GetConfigManager(), "HideRoom") });

    if (SDK::LevelController_sFields->instance && SDK::LevelController_sFields->instance->Fields.currentGhostRoom)
        if (const auto ghostRoom = SDK::LevelController_sFields->instance->Fields.currentGhostRoom; ghostRoom->Fields.RoomName)
            rows.push_back({ "Location", LANG("Location"), Utils::UnityStrToSysStr(*ghostRoom->Fields.RoomName), CONFIG_BOOL(GetConfigManager(), "HideLocation") });
    if (const auto& ghostNavMesh = Utils::GetGhostAI()->Fields.NavMeshAgent)
        rows.push_back({ "GhostCurrentSpeed", LANG("GhostCurrentSpeed"), std::format("{:.1f} units/s", SDK::NavMeshAgent_get_speed(ghostNavMesh, nullptr)), CONFIG_BOOL(GetConfigManager(), "HideCurrentSpeed") }); // unity units/seconds
    if (Globals::isHunting && InGame::huntingState && InGame::huntingState->Fields.huntDurationTimer > 0)
        rows.push_back({ "HuntDuration", LANG("GhostHuntDuration"), std::format("{:.1f} sec", InGame::huntingState->Fields.huntDurationTimer), CONFIG_BOOL(GetConfigManager(), "HideHuntDuration")});
	if (secondsSinceSmudge > 0.f)
		rows.push_back({ "GhostSmudged", LANG("GhostSmudged"), std::format("{:.1f} sec", secondsSinceSmudge.load()), CONFIG_BOOL(GetConfigManager(), "HideGhostSmudged") });
	if (ghostAfterHunting > 0.f)
		rows.push_back({ "GhostAfterHunting", LANG("GhostAfterHunting"), std::format("{:.1f} sec", ghostAfterHunting.load()), CONFIG_BOOL(GetConfigManager(), "HideGhostAfterHunting") });

    DrawReorderableRows(rows);

    ImGui::Separator();
    ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.9f, 1.0f), "Status");
    ImGui::SameLine(140.0f);
    if (Globals::isHunting)
        ImGui::TextColored(ImVec4(1.0f, 0.4f, 0.4f, 1.0f), "HUNTING");
    else
        ImGui::TextColored(ImVec4(0.4f, 1.0f, 0.4f, 1.0f), "PASSIVE");

    ImGui::End();
    ImGui::PopStyleVar(3);
}

void GhostPanel::OnMenuRender()
{
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 6));

    bool enabled = IsActive();
    if (ImGui::Checkbox(LANG("EnableGhostPanel"), &enabled))
    {
        SET_CONFIG_VALUE(GetConfigManager(), "Enabled", bool, enabled);
        if (enabled) OnActivate();
        else OnDeactivate();
    }

    if (enabled) {
        bool bansheeTarget = CONFIG_BOOL(GetConfigManager(), "BansheeTargetSetting");

        if (ImGui::Checkbox((std::string(LANG("ShowBansheeTarget")) + "##ghostPanel_banshee").c_str(), &bansheeTarget))
            SET_CONFIG_VALUE(GetConfigManager(), "BansheeTargetSetting", bool, bansheeTarget);

        bool IsHideSettings = CONFIG_BOOL(GetConfigManager(), "IsHideSettings");
        if (ImGui::Checkbox(LANG("IsHideSettingsTr"), &IsHideSettings))
            SET_CONFIG_VALUE(GetConfigManager(), "IsHideSettings", bool, IsHideSettings);

        if (IsHideSettings)
        {
            bool hideName = CONFIG_BOOL(GetConfigManager(), "HideName");
            if (ImGui::Checkbox(LANG("HideName"), &hideName))
                SET_CONFIG_VALUE(GetConfigManager(), "HideName", bool, hideName);

            bool hideType = CONFIG_BOOL(GetConfigManager(), "HideType");
            if (ImGui::Checkbox(LANG("HideType"), &hideType))
                SET_CONFIG_VALUE(GetConfigManager(), "HideType", bool, hideType);

            bool hideAge = CONFIG_BOOL(GetConfigManager(), "HideAge");
            if (ImGui::Checkbox(LANG("HideAge"), &hideAge))
                SET_CONFIG_VALUE(GetConfigManager(), "HideAge", bool, hideAge);

            bool hideState = CONFIG_BOOL(GetConfigManager(), "HideState");
            if (ImGui::Checkbox(LANG("HideState"), &hideState))
                SET_CONFIG_VALUE(GetConfigManager(), "HideState", bool, hideState);

            bool hideEvidence = CONFIG_BOOL(GetConfigManager(), "HideEvidence");
            if (ImGui::Checkbox(LANG("HideEvidence"), &hideEvidence))
                SET_CONFIG_VALUE(GetConfigManager(), "HideEvidence", bool, hideEvidence);

            bool hideRoom = CONFIG_BOOL(GetConfigManager(), "HideRoom");
            if (ImGui::Checkbox(LANG("HideRoom"), &hideRoom))
                SET_CONFIG_VALUE(GetConfigManager(), "HideRoom", bool, hideRoom);

            bool hideLocation = CONFIG_BOOL(GetConfigManager(), "HideLocation");
            if (ImGui::Checkbox(LANG("HideLocation"), &hideLocation))
                SET_CONFIG_VALUE(GetConfigManager(), "HideLocation", bool, hideLocation);

            bool hideMimic = CONFIG_BOOL(GetConfigManager(), "HideMimicType");
            if (ImGui::Checkbox(LANG("HideMimicType"), &hideMimic))
                SET_CONFIG_VALUE(GetConfigManager(), "HideMimicType", bool, hideMimic);

            bool hideTarget = CONFIG_BOOL(GetConfigManager(), "HideBansheeTarget");
            if (ImGui::Checkbox(LANG("HideBansheeTarget"), &hideTarget))
                SET_CONFIG_VALUE(GetConfigManager(), "HideBansheeTarget", bool, hideTarget);

            bool hideCurrentSpeed = CONFIG_BOOL(GetConfigManager(), "HideCurrentSpeed");
            if (ImGui::Checkbox(LANG("HideCurrentSpeed"), &hideCurrentSpeed))
                SET_CONFIG_VALUE(GetConfigManager(), "HideCurrentSpeed", bool, hideCurrentSpeed);

            bool hideHuntDuration = CONFIG_BOOL(GetConfigManager(), "HideHuntDuration");
            if (ImGui::Checkbox(LANG("HideHuntDuration"), &hideHuntDuration))
                SET_CONFIG_VALUE(GetConfigManager(), "HideHuntDuration", bool, hideHuntDuration);

			bool hideGhostSmudged = CONFIG_BOOL(GetConfigManager(), "HideGhostSmudged");
			if (ImGui::Checkbox(LANG("HideGhostSmudged"), &hideGhostSmudged))
				SET_CONFIG_VALUE(GetConfigManager(), "HideGhostSmudged", bool, hideGhostSmudged);

			bool hideGhostAfterHunting = CONFIG_BOOL(GetConfigManager(), "HideGhostAfterHunting");
			if (ImGui::Checkbox(LANG("HideGhostAfterHunting"), &hideGhostAfterHunting))
				SET_CONFIG_VALUE(GetConfigManager(), "HideGhostAfterHunting", bool, hideGhostAfterHunting);
        }
    }

    ImGui::PopStyleVar();
}

std::string GhostPanel::GetGhostEvidenceString()
{
    const auto& ghostInfo = Utils::GetGhostAI()->Fields.GhostInfo;
    const auto& ghostEvidenceList = ghostInfo->Fields.GhostTraits.GhostEvidenceList;

    if (!ghostEvidenceList || ghostEvidenceList->Fields.Size == 0)
        return {};

    std::string evidence;
    const auto items = ghostEvidenceList->Fields.Items->Vector;

    for (auto i = 0; i < ghostEvidenceList->Fields.Size; ++i)
    {
        if (i > 0)
            evidence += " | ";

        evidence += Utils::GhostEnumToStrLocalized(items[i]);
    }

    return evidence;
}

void GhostPanel::GhostTimerHook(float seconds, int code)
{
    std::atomic<float>* timer = nullptr;

    if (code == 1)
        timer = &secondsSinceSmudge;
    else if (code == 2)
        timer = &ghostAfterHunting;
    else
        return;

    std::thread([timer, seconds]()
        {
            auto remaining = seconds;

            while (remaining > 0.0f)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));

                remaining -= 0.1f;

                if (remaining < 0.0f)
                    remaining = 0.0f;

                timer->store(remaining);
            }

            timer->store(0.0f);
        }).detach();
}