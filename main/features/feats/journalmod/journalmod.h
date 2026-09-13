#pragma once
#include "../Includes.h"

namespace PhasmoCheatV::Features::Misc
{
	class JournalModifier final : public FeatureCore
	{
	public:
		JournalModifier();
		~JournalModifier() override = default;

		void OnActivate() override;
		void OnDeactivate() override;
		void OnRender() override;
		void OnMenuRender() override;
		void JournalModifierHandler();

	private:
		float lastUpdateTime = 0.0f;

		void JournalModifierMain(int GhostType);
	};
}
