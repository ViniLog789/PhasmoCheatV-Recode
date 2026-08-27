#include "actmonik.h"

using namespace PhasmoCheatV::Features::Visuals;

ActivityMonitor::ActivityMonitor() : FeatureCore(LANG("ActivityMonitor_Header"), TYPE_VISUALS) {}

void ActivityMonitor::OnRender()
{
	if (!IsActive())
		return;

    if (!Utils::IsInGame())
        return;

    if (!SDK::EMFData_sFields->instance || !SDK::EMFData_sFields->instance->Fields.Rend)
        return;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16.0f, 16.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8.0f, 12.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8.0f, 6.0f));

    ImGui::SetNextWindowSize(ImVec2(450.0f, 300.0f), ImGuiCond_Once);
    ImGui::Begin("Activity Monitor", nullptr, Globals::WINDOW_FLAGS_GLOBALS);

    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
    ImGui::TextColored(ImVec4(0.8f, 0.6f, 1.0f, 1.0f), "GHOST ACTIVITY");
    ImGui::PopFont();

    ImGui::Separator();
    ImGui::Spacing();

    constexpr auto chartDimensions = ImVec2(400.0f, 200.0f);
    const ImVec2 chartPosition = ImGui::GetCursorScreenPos();
    ImDrawList* renderList = ImGui::GetWindowDrawList();

    constexpr int horizontalDivisions = chartDimensions.x / 6;
    constexpr int verticalDivisions = chartDimensions.y / 10;
    constexpr ImU32 divisionColor = IM_COL32(80, 80, 100, 80);
    constexpr ImU32 borderColor = IM_COL32(120, 120, 140, 200);

    for (int xCoord = 0; xCoord <= chartDimensions.x; xCoord += horizontalDivisions)
    {
        renderList->AddLine(
            ImVec2(chartPosition.x + xCoord, chartPosition.y),
            ImVec2(chartPosition.x + xCoord, chartPosition.y + chartDimensions.y),
            divisionColor
        );
    }

    for (int yCoord = 0; yCoord <= chartDimensions.y; yCoord += verticalDivisions)
    {
        renderList->AddLine(
            ImVec2(chartPosition.x, chartPosition.y + yCoord),
            ImVec2(chartPosition.x + chartDimensions.x, chartPosition.y + yCoord),
            divisionColor
        );
    }

    renderList->AddLine(
        ImVec2(chartPosition.x, chartPosition.y),
        ImVec2(chartPosition.x, chartPosition.y + chartDimensions.y),
        borderColor, 2.0f
    );

    renderList->AddLine(
        ImVec2(chartPosition.x, chartPosition.y + chartDimensions.y),
        ImVec2(chartPosition.x + chartDimensions.x, chartPosition.y + chartDimensions.y),
        borderColor, 2.0f
    );

    std::vector<ImVec2> dataPoints;
    dataPoints.reserve(60);

    for (int index = 0; index < 60; ++index)
    {
        const SDK::Vector3 location = SDK::LineRenderer_GetPosition(SDK::EMFData_sFields->instance->Fields.Rend, index, nullptr);
        float scaledY = location.Y / 48.0f;
        float boundedY = std::clamp(scaledY, 0.0f, 10.0f);
        float plotY = chartPosition.y + chartDimensions.y - (boundedY * (chartDimensions.y / 10.0f));
        plotY = (std::min)(plotY, chartPosition.y + chartDimensions.y);
        plotY = (std::max)(plotY, chartPosition.y);

        dataPoints.emplace_back(
            chartPosition.x + static_cast<float>(index) * (chartDimensions.x / 60.0f),
            plotY
        );
    }

    renderList->AddPolyline(
        dataPoints.data(),
        dataPoints.size(),
        IM_COL32(220, 100, 100, 255),
        false,
        2.5f
    );

    for (int timeMark = 0; timeMark <= 60; timeMark += 10)
    {
        std::string timeText = std::to_string(timeMark) + "s";
        renderList->AddText(
            ImVec2(
                chartPosition.x + timeMark * (chartDimensions.x / 60.0f) - 8.0f,
                chartPosition.y + chartDimensions.y + 8.0f
            ),
            IM_COL32(200, 200, 220, 255),
            timeText.c_str()
        );
    }

    for (int valueMark = 0; valueMark <= 10; valueMark += 2)
    {
        std::string valueText = std::to_string(valueMark);
        renderList->AddText(
            ImVec2(
                chartPosition.x - 25.0f,
                chartPosition.y + chartDimensions.y - valueMark * (chartDimensions.y / 10.0f) - 8.0f
            ),
            IM_COL32(200, 200, 220, 255),
            valueText.c_str()
        );
    }

    ImGui::Dummy(ImVec2(chartDimensions.x, chartDimensions.y + 25.0f));
    ImGui::End();
    ImGui::PopStyleVar(3);
}

void ActivityMonitor::OnMenuRender()
{
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8.0f, 6.0f));

    bool isEnabled = IsActive();
    if (ImGui::Checkbox(LANG("EnableActivityMonitor"), &isEnabled))
    {
        SET_CONFIG_VALUE(GetConfigManager(), "Enabled", bool, isEnabled);
        if (isEnabled) OnActivate();
        else OnDeactivate();
    }

    ImGui::PopStyleVar();
}