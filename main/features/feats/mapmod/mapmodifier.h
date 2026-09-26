#pragma once
#include "../Includes.h"

namespace PhasmoCheatV::Features::Map
{
    class MapModifier final : public FeatureCore
    {
    public:
        MapModifier();
        ~MapModifier() override = default;
        void OnActivate() override {}
        void OnDeactivate() override {}
        void OnRender() override {}
        void OnMenuRender() override;
        void MapModifierMain();

    private:
        int32_t lightsModifier = 0; // 0 - false function, 1 - On, 2 - off
        bool callLightning = false;
        bool switchFuseBox = false;
    };
}
