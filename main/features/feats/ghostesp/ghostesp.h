#pragma once
#include "../Includes.h"

namespace PhasmoCheatV::Features::Visuals
{
    class GhostESP final : public FeatureCore
    {
    public:
        GhostESP();
        ~GhostESP() override = default;

        void OnActivate() override {}
        void OnDeactivate() override {}
        void OnRender() override;
        void OnMenuRender() override;
        static void SetD3D11Device(ID3D11Device* device);

    private:
        struct ProjectionContext
        {
            SDK::Camera* Camera = nullptr;
            float ScreenHeight = 0.0f;
        };

        struct BodySnapshot
        {
            SDK::Vector3 TopScreen{};
            SDK::Vector3 BottomScreen{};
            bool Valid = false;
        };

        static bool BuildProjectionContext(ProjectionContext& out);
        static bool ProjectWorldToScreen(const ProjectionContext& context, const SDK::Vector3& worldPos, SDK::Vector3& screenPos);
        static bool BuildBodySnapshot(const SDK::GhostAI* ghostAI, const ProjectionContext& projection, BodySnapshot& out);
        void Draw2DBox(const BodySnapshot& body, const ImColor& color, float thickness);
        void DrawCornerBox(const BodySnapshot& body, const ImColor& color, float thickness);
        void DrawFilledBox(const BodySnapshot& body, const ImColor& color, float thickness);
        void DrawName(const SDK::GhostAI* ghostAI, const BodySnapshot& body, const ImColor& color);
        void DrawSkeleton(const SDK::GhostAI* ghostAI, const ProjectionContext& projection, const ImColor& color, float thickness);
        void DrawPhotoESP(const BodySnapshot& body);
        void LoadAvailablePhotos();
        bool LoadTexture(const std::string& filename, ID3D11ShaderResourceView** outSrv, int* outWidth, int* outHeight);
        void CleanupTextures();

        struct TextureData
        {
            ID3D11ShaderResourceView* SRV = nullptr;
            int Width = 0;
            int Height = 0;
        };

        std::unordered_map<std::string, TextureData> loadedTextures;
        std::vector<std::string> availablePhotos;

        static ID3D11Device* g_pd3dDevice;
    };
}
