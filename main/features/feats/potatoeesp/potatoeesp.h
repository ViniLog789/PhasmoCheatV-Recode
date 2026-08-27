#pragma once
#include "../Includes.h"

namespace PhasmoCheatV::Features::Visuals
{
    class PotatoeEsp final : public FeatureCore
    {
    public:
        PotatoeEsp();
        ~PotatoeEsp() override = default;

        void OnActivate() override;
        void OnDeactivate() override;
        void OnRender() override;
        void OnMenuRender() override;

    private:
		SDK::Transform* cachedPotatoeTransform = nullptr;
    };
}