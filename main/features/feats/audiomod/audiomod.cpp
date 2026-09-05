#include "audiomod.h"

using namespace PhasmoCheatV::Features::Misc;

AudioModifier::AudioModifier() : FeatureCore(LANG("AudioModifier_Header"), TYPE_MISC) 
{
	DECLARE_CONFIG(GetConfigManager(), "MuteWeather", bool, false);
	DECLARE_CONFIG(GetConfigManager(), "MuteFootsteps", bool, false);
}

void AudioModifier::OnMenuRender()
{
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 6));

	bool enabled = IsActive();
	if (ImGui::Checkbox(LANG("AudioModifierEnable"), &enabled))
	{
		SET_CONFIG_VALUE(GetConfigManager(), "Enabled", bool, enabled);
		if (enabled) OnActivate();
		else OnDeactivate();
	}

	if (!enabled)
	{
		ImGui::PopStyleVar();
		return;
	}

	bool muteWeather = CONFIG_BOOL(GetConfigManager(), "MuteWeather");
	if (ImGui::Checkbox(LANG("DisableWeatherAudio"), &muteWeather))
		SET_CONFIG_VALUE(GetConfigManager(), "MuteWeather", bool, muteWeather);

	bool muteFootsteps = CONFIG_BOOL(GetConfigManager(), "MuteFootsteps");
	if (ImGui::Checkbox(LANG("DisableFootstepAudio"), &muteFootsteps))
		SET_CONFIG_VALUE(GetConfigManager(), "MuteFootsteps", bool, muteFootsteps);

	ImGui::PopStyleVar();
}

bool AudioModifier::SetVolume(SDK::AudioSource* audio, float volume, float* lastValue)
{
	if (!audio || !SDK::AudioSource_set_volume) return false;
	
	float prevval = SDK::AudioSource_get_volume(audio, nullptr);

	if (volume == prevval) return true;

	if (lastValue)
		*lastValue = prevval;
	
	SDK::AudioSource_set_volume(audio, volume, nullptr);
	return true;
}

void AudioModifier::AudioModifierHandler()
{
	if (!IsActive()) return;

	SDK::Object* gameObject = reinterpret_cast<SDK::Object*>(
		SDK::Component_Get_GameObject(
			reinterpret_cast<SDK::Component*>(cachedAzure),
			nullptr
		)
		);
	bool IsNativeAlive = SDK::Object_IsNativeAlive(gameObject, nullptr);

	if (!cachedAzure || !IsNativeAlive)
		cachedAzure = reinterpret_cast<SDK::AzureEffectsController*>(Utils::FindComponentByType("UnityEngine.AzureSky.AzureEffectsController", true));

	if (!cachedAzure) return;

	auto* lightRainAudioSource = cachedAzure->Fields.lightRainAudioSource;
	auto* heavyWindAudioSource = cachedAzure->Fields.heavyWindAudioSource;
	auto* heavyRainAudioSource = cachedAzure->Fields.heavyRainAudioSource;
	if (!lightRainAudioSource || !heavyWindAudioSource || !heavyRainAudioSource)
		return;

	bool muteWeather = CONFIG_BOOL(GetConfigManager(), "MuteWeather");

	if (!IsActive())
	{
		if (basevol1 != 0.f || basevol2 != 0.f || basevol3 != 0.f)
		{
			SetVolume(lightRainAudioSource, basevol1, nullptr);
			SetVolume(heavyWindAudioSource, basevol2, nullptr);
			SetVolume(heavyRainAudioSource, basevol3, nullptr);
		}
		return;
	}

	if (muteWeather)
	{
		SetVolume(lightRainAudioSource, 0.f, &basevol1);
		SetVolume(heavyWindAudioSource, 0.f, &basevol2);
		SetVolume(heavyRainAudioSource, 0.f, &basevol3);
	}
}