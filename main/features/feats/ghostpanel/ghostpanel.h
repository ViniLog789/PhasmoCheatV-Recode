#pragma once
#include "../Includes.h"

namespace PhasmoCheatV::Features::Visuals
{
	class GhostPanel final : public FeatureCore
	{
	public:
		GhostPanel();
		~GhostPanel() override = default;

		void OnActivate() override {}
		void OnDeactivate() override {}
		void OnRender() override;
		void OnMenuRender() override;
		void DrawHiddenValue(float width = 120.0f, float height = 18.0f);
		void GhostTimerHook(float seconds, int code); // code 1 - smudged, code 2 - hunting cooldown

	private:
		static std::string GetGhostEvidenceString();

		struct RowDef {
			std::string id;
			std::string label;
			std::string value;
			bool hidden;
		};

		std::vector<std::string> m_rowOrder;
		bool m_rowOrderLoaded = false;
		void LoadRowOrder();
		void SaveRowOrder();
		void DrawReorderableRows(std::vector<RowDef>& rows);

		std::atomic<float> secondsSinceSmudge{ 0.0f };
		std::atomic<float> ghostAfterHunting{ 0.0f };
	};
}
