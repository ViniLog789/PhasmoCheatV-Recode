#include "potatoeesp.h"

using namespace PhasmoCheatV::Features::Visuals;

PotatoeEsp::PotatoeEsp() : FeatureCore(LANG("PotatoeESP_Header"), TYPE_VISUALS)
{
    DECLARE_CONFIG(GetConfigManager(), "Color", ImColor, ImColor(255, 255, 255, 255));
}

void PotatoeEsp::OnActivate()
{
	cachedPotatoeTransform = Utils::GetPotatoe();
}

void PotatoeEsp::OnDeactivate()
{
	cachedPotatoeTransform = nullptr;
}

void PotatoeEsp::OnRender()
{
    if (!IsActive() || !Utils::GetMap() 
        || Utils::GetMap()->Fields.uniqueMapID != 4 || !Utils::GetGhostAI())
    {
        return;
    }

	if (!cachedPotatoeTransform)
	{
		cachedPotatoeTransform = Utils::GetPotatoe();
		if (!cachedPotatoeTransform)
			return;
	}

    const ImColor color = CONFIG_COLOR(GetConfigManager(), "Color");

    SDK::Vector3 worldPos = Utils::GetPosVec3(cachedPotatoeTransform);

    SDK::Vector3 screenPos;
    if (!Utils::WTS(worldPos, screenPos) || screenPos.Z <= 0)
        return;

    auto draw = ImGui::GetBackgroundDrawList();

    const char* label = "Potatoe";
    ImVec2 textSize = ImGui::CalcTextSize(label);

    float yOffset = 20.0f;

    draw->AddText(
        ImVec2(
            screenPos.X - textSize.x * 0.5f,
            screenPos.Y - yOffset
        ),
        color,
        label
    );
}

void PotatoeEsp::OnMenuRender()
{
    bool enabled = IsActive();
    std::string enabledLabel = std::string(LANG("PotatoeESPEn")) + "###potatoeESP";

    if (ImGui::Checkbox(enabledLabel.c_str(), &enabled))
    {
        SET_CONFIG_VALUE(GetConfigManager(), "Enabled", bool, enabled);
        if (enabled) OnActivate();
        else OnDeactivate();
    }

    if (!enabled)
        return;

    ImGui::SameLine();

    ImColor currentColor = CONFIG_COLOR(GetConfigManager(), "Color");
    if (ImGui::ColorEdit4(
        "###potatoeESPColor",
        reinterpret_cast<float*>(&currentColor.Value),
        ImGuiColorEditFlags_NoInputs
    ))
    {
        SET_CONFIG_VALUE(GetConfigManager(), "Color", ImColor, currentColor);
    }
}