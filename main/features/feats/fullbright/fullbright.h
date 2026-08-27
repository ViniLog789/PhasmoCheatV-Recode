#pragma once
#include "../Includes.h"

namespace PhasmoCheatV::Features::Visuals
{
	class Fullbright final : public FeatureCore
	{
	public:
		explicit Fullbright();
		~Fullbright() override = default;

		void OnActivate() override;
		void OnDeactivate() override;
		void OnRender() override {}
		void OnMenuRender() override;
		void FullbrightMain();
		void BoostDarkMaterials();
		void BoostPlayerLights();

	private:
		bool m_bleasdaleInitDone = false;
		SDK::String* m_fbNameStr = nullptr;
		SDK::GameObject* m_lightGO = nullptr;
		SDK::Light* m_lightComp = nullptr;
		SDK::Type* m_cachedLightType = nullptr;
		void Reset() {
			m_lightGO = nullptr;
			m_lightComp = nullptr;
			m_bleasdaleInitDone = false;
		}
	};
}
