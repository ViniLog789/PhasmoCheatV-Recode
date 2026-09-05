#pragma once
#include "../Includes.h"

namespace PhasmoCheatV::Features::Misc
{
	class AudioModifier : public FeatureCore
	{
	public:
		AudioModifier();
		~AudioModifier() override = default;

		void OnActivate() override {};
		void OnDeactivate() override {};
		void OnRender() override {};
		void OnMenuRender() override;
		void AudioModifierHandler();
		bool get_bMuteFootsteps() { return CONFIG_BOOL(GetConfigManager(), "MuteFootsteps"); }
	private:
		bool SetVolume(SDK::AudioSource* audio, float volume, float* lastValue = nullptr);
		float basevol1;
		float basevol2;
		float basevol3;
		SDK::AzureEffectsController* cachedAzure = nullptr;
	};
}