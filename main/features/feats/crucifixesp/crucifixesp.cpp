#include "crucifixesp.h"

using namespace PhasmoCheatV::Features::Visuals;

CrucifixESP::CrucifixESP() : FeatureCore(LANG("CrucifixESP_Header"), TYPE_VISUALS)
{
	DECLARE_CONFIG(GetConfigManager(), "ShowUses", bool, true);
	DECLARE_CONFIG(GetConfigManager(), "ShowName", bool, true);
	DECLARE_CONFIG(GetConfigManager(), "ShowRangeM", bool, false);
	DECLARE_CONFIG(GetConfigManager(), "ShowStoppedCHunts", bool, false);
}

std::vector<SDK::Crucifix*> CrucifixESP::getCruxifixs()
{
	std::vector<SDK::Crucifix*> crucifixs;

	auto levelController = SDK::LevelController_sFields->instance;
	if (!levelController)
		return crucifixs;

	auto crucifixList = levelController->Fields.crucifix;
	if (!crucifixList)
		return crucifixs;

	auto crucifixArray = crucifixList->Fields._items;
	if (!crucifixArray)
		return crucifixs;

	auto size = crucifixList->Fields._size;
	if (size <= 0)
		return crucifixs;

	for (int32_t i = 0; i < size; i++)
	{
		auto t_crucifix = crucifixArray->Vector[i];
		if (!t_crucifix)
			continue;

		crucifixs.push_back(t_crucifix);
	}

	return crucifixs;
}

void CrucifixESP::OnRender()
{
	auto crucifixs = getCruxifixs();
	if (crucifixs.empty())
		return;

	bool showUses = CONFIG_BOOL(GetConfigManager(), "ShowUses");
	bool showName = CONFIG_BOOL(GetConfigManager(), "ShowName");
	bool showRange = CONFIG_BOOL(GetConfigManager(), "ShowRangeM");
	bool showStoppedCHunts = CONFIG_BOOL(GetConfigManager(), "ShowStoppedCHunts");

	for (auto crucifix : crucifixs)
	{
		if (!crucifix)
			continue;

		auto transform = SDK::Component_Get_Transform(reinterpret_cast<SDK::Component*>(crucifix), nullptr);
		if (!transform)
			continue;

		auto vec3Pos = SDK::Transform_Get_Position(transform, nullptr);

		SDK::Vector3 displayPos;
		bool w2s = Utils::WTS(vec3Pos, displayPos);

		if (w2s)
		{
			auto draw = ImGui::GetBackgroundDrawList();
			auto font = ImGui::GetFont();

			struct EspLine {
				std::string text;
				ImU32 color;
			};

			EspLine lines[4];
			int lineCount = 0;

			if (showName)
			{
				lines[lineCount++] = { "Crucifix", IM_COL32(200, 200, 255, 255) };
			}

			if (showUses)
			{
				int uses = crucifix->Fields.useCount;
				int maxUses = crucifix->Fields.maxUses;

				ImU32 usesColor = (uses >= maxUses) ? IM_COL32(255, 75, 75, 255) : IM_COL32(75, 255, 75, 255);
				lines[lineCount++] = { "Uses: " + std::to_string(uses) + " / " + std::to_string(maxUses), usesColor };
			}

			if (showRange)
				lines[lineCount++] = { "Range: " + std::to_string(crucifix->Fields.range) + "m", IM_COL32(220, 220, 220, 255) };

			if (showStoppedCHunts)
			{
				bool stopped = crucifix->Fields.stoppedCursedHunt;
				ImU32 huntColor = stopped ? IM_COL32(255, 165, 0, 255) : IM_COL32(200, 200, 200, 255);
				lines[lineCount++] = { std::string("Stopped C. Hunt: ") + (stopped ? "Yes" : "No"), huntColor };
			}

			if (lineCount == 0)
				continue;

			float totalHeight = 0.0f;
			for (int i = 0; i < lineCount; ++i)
				totalHeight += font->CalcTextSizeA(font->LegacySize, FLT_MAX, 0.0f, lines[i].text.c_str()).y;

			float currentY = displayPos.Y - (totalHeight * 0.5f);
			ImU32 outlineColor = IM_COL32(0, 0, 0, 255);

			for (int i = 0; i < lineCount; ++i)
			{
				ImVec2 textSize = font->CalcTextSizeA(font->LegacySize, FLT_MAX, 0.0f, lines[i].text.c_str());

				ImVec2 textPos(displayPos.X - (textSize.x * 0.5f), currentY);

				const char* c_str = lines[i].text.c_str();

				draw->AddText(font, font->LegacySize, ImVec2(textPos.x + 1, textPos.y + 1), outlineColor, c_str);
				draw->AddText(font, font->LegacySize, ImVec2(textPos.x - 1, textPos.y - 1), outlineColor, c_str);
				draw->AddText(font, font->LegacySize, ImVec2(textPos.x + 1, textPos.y - 1), outlineColor, c_str);
				draw->AddText(font, font->LegacySize, ImVec2(textPos.x - 1, textPos.y + 1), outlineColor, c_str);

				draw->AddText(font, font->LegacySize, textPos, lines[i].color, c_str);

				currentY += textSize.y;
			}
		}
	}
}

void CrucifixESP::OnMenuRender()
{
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 6));

	bool enabled = IsActive();
	if (ImGui::Checkbox(LANG("EnableCrucifixESP"), &enabled))
	{
		SET_CONFIG_VALUE(GetConfigManager(), "Enabled", bool, enabled);
		enabled ? OnActivate() : OnDeactivate();
	}

	if (enabled)
	{
		auto DrawConfigCheckbox = [&](const char* langKey, const char* configKey) {
			bool val = CONFIG_BOOL(GetConfigManager(), configKey);
			if (ImGui::Checkbox(LANG(langKey), &val))
				SET_CONFIG_VALUE(GetConfigManager(), configKey, bool, val);
			};

		DrawConfigCheckbox("ShowUsesCheckbox", "ShowUses");
		DrawConfigCheckbox("ShowNameCheckbox", "ShowName");
		DrawConfigCheckbox("ShowRangeMCheckbox", "ShowRangeM");
		DrawConfigCheckbox("ShowStoppedCHuntsCheckbox", "ShowStoppedCHunts");
	}

	ImGui::PopStyleVar();
}