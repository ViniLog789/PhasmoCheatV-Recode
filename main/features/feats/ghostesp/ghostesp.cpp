#include "ghostesp.h"

#include <array>
#include <cmath>
#include <filesystem>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include "imgui/stb_image.h"

using namespace PhasmoCheatV::Features::Visuals;

namespace
{
    using Bone = SDK::HumanBodyBones;

    constexpr std::array<std::pair<Bone, Bone>, 20> kMainConnections = {
        std::pair{ Bone::Hips, Bone::Spine },
        std::pair{ Bone::Spine, Bone::Chest },
        std::pair{ Bone::Chest, Bone::Neck },
        std::pair{ Bone::Neck, Bone::Head },
        std::pair{ Bone::Chest, Bone::LeftShoulder },
        std::pair{ Bone::LeftShoulder, Bone::LeftUpperArm },
        std::pair{ Bone::LeftUpperArm, Bone::LeftLowerArm },
        std::pair{ Bone::LeftLowerArm, Bone::LeftHand },
        std::pair{ Bone::Chest, Bone::RightShoulder },
        std::pair{ Bone::RightShoulder, Bone::RightUpperArm },
        std::pair{ Bone::RightUpperArm, Bone::RightLowerArm },
        std::pair{ Bone::RightLowerArm, Bone::RightHand },
        std::pair{ Bone::Hips, Bone::LeftUpperLeg },
        std::pair{ Bone::LeftUpperLeg, Bone::LeftLowerLeg },
        std::pair{ Bone::LeftLowerLeg, Bone::LeftFoot },
        std::pair{ Bone::LeftFoot, Bone::LeftToes },
        std::pair{ Bone::Hips, Bone::RightUpperLeg },
        std::pair{ Bone::RightUpperLeg, Bone::RightLowerLeg },
        std::pair{ Bone::RightLowerLeg, Bone::RightFoot },
        std::pair{ Bone::RightFoot, Bone::RightToes }
    };

    constexpr std::array<Bone, 21> kMainBones = {
        Bone::Hips,
        Bone::Spine,
        Bone::Chest,
        Bone::Neck,
        Bone::Head,
        Bone::LeftShoulder,
        Bone::LeftUpperArm,
        Bone::LeftLowerArm,
        Bone::LeftHand,
        Bone::RightShoulder,
        Bone::RightUpperArm,
        Bone::RightLowerArm,
        Bone::RightHand,
        Bone::LeftUpperLeg,
        Bone::LeftLowerLeg,
        Bone::LeftFoot,
        Bone::LeftToes,
        Bone::RightUpperLeg,
        Bone::RightLowerLeg,
        Bone::RightFoot,
        Bone::RightToes
    };

    constexpr size_t kBoneSlotCount =
        static_cast<size_t>(SDK::HumanBodyBones::LastBone);

    struct BoneScreenData
    {
        SDK::Vector3 Screen{};
        bool Visible = false;
    };

    using BoneScreenCache = std::array<BoneScreenData, kBoneSlotCount>;

    constexpr size_t BoneIndex(Bone bone)
    {
        return static_cast<size_t>(bone);
    }
}

ID3D11Device* GhostESP::g_pd3dDevice = nullptr;

GhostESP::GhostESP() : FeatureCore(LANG("GhostESP_Header"), TYPE_VISUALS)
{
    DECLARE_CONFIG(GetConfigManager(), "ESPColor", ImColor, ImColor(255, 0, 0, 255));
    DECLARE_CONFIG(GetConfigManager(), "BoxType", int, 0);
    DECLARE_CONFIG(GetConfigManager(), "BoxThickness", float, 1.5f);
    DECLARE_CONFIG(GetConfigManager(), "ShowSkeleton", bool, true);
    DECLARE_CONFIG(GetConfigManager(), "SkeletonColor", ImColor, ImColor(255, 255, 255, 255));
    DECLARE_CONFIG(GetConfigManager(), "NameCol", ImColor, ImColor(255, 255, 255, 255));
    DECLARE_CONFIG(GetConfigManager(), "SkeletonThickness", float, 1.0f);
    DECLARE_CONFIG(GetConfigManager(), "ShowPhotoESP", bool, false);
    DECLARE_CONFIG(GetConfigManager(), "ShowBoxESP", bool, false);
    DECLARE_CONFIG(GetConfigManager(), "PhotoESPType", int, 0);
    DECLARE_CONFIG(GetConfigManager(), "PhotoWidth", float, 1.0f);
    DECLARE_CONFIG(GetConfigManager(), "PhotoHeight", float, 1.0f);

    LoadAvailablePhotos();
}

bool GhostESP::BuildProjectionContext(ProjectionContext& out)
{
    out = {};

    const auto* localPlayer = Utils::GetLocalPlayer();
    if (!localPlayer || !localPlayer->Fields.LocalPlayer)
        return false;

    out.Camera = localPlayer->Fields.LocalPlayer->Fields.Camera;
    if (!out.Camera)
        return false;

    out.ScreenHeight = static_cast<float>(SDK::Screen_Get_Height(nullptr));
    return out.ScreenHeight > 0.0f;
}

bool GhostESP::ProjectWorldToScreen(const ProjectionContext& context, const SDK::Vector3& worldPos, SDK::Vector3& screenPos)
{
    screenPos = {};

    if (!context.Camera || context.ScreenHeight <= 0.0f)
        return false;

    const SDK::Vector3 projected =
        SDK::Camera_WorldToScreenPoint(context.Camera, worldPos, nullptr);

    if (projected.Z <= 0.0f)
        return false;

    screenPos = {
        projected.X,
        context.ScreenHeight - projected.Y,
        projected.Z
    };

    return true;
}

bool GhostESP::BuildBodySnapshot(const SDK::GhostAI* ghostAI, const ProjectionContext& projection, BodySnapshot& out)
{
    out = {};

    if (!ghostAI || !ghostAI->Fields.raycastPoint || !ghostAI->Fields.feetRaycastPoint)
        return false;

    const SDK::Vector3 topWorld =
        Utils::GetPosVec3(ghostAI->Fields.raycastPoint);
    const SDK::Vector3 bottomWorld =
        Utils::GetPosVec3(ghostAI->Fields.feetRaycastPoint);

    if (!ProjectWorldToScreen(projection, topWorld, out.TopScreen) ||
        !ProjectWorldToScreen(projection, bottomWorld, out.BottomScreen))
    {
        return false;
    }

    out.Valid = true;
    return true;
}

void GhostESP::OnRender()
{
    if (!IsActive() || !Utils::IsInGame())
        return;

    const auto* ghostAI = Utils::GetGhostAI();
    if (!ghostAI || !ghostAI->Fields.GhostInfo)
        return;

    ProjectionContext projection;
    if (!BuildProjectionContext(projection))
        return;

    BodySnapshot body;
    if (!BuildBodySnapshot(ghostAI, projection, body))
        return;

    const ImColor nameColor = CONFIG_COLOR(GetConfigManager(), "NameCol");
    const bool showBox = CONFIG_BOOL(GetConfigManager(), "ShowBoxESP");
    const bool showSkeleton = CONFIG_BOOL(GetConfigManager(), "ShowSkeleton");
    const bool showPhoto = CONFIG_BOOL(GetConfigManager(), "ShowPhotoESP");

    DrawName(ghostAI, body, nameColor);

    if (showBox)
    {
        const ImColor boxColor = CONFIG_COLOR(GetConfigManager(), "ESPColor");
        const float boxThickness = CONFIG_FLOAT(GetConfigManager(), "BoxThickness");

        switch (CONFIG_INT(GetConfigManager(), "BoxType"))
        {
        case 0:
            Draw2DBox(body, boxColor, boxThickness);
            break;
        case 1:
            DrawCornerBox(body, boxColor, boxThickness);
            break;
        case 2:
            DrawFilledBox(body, boxColor, boxThickness);
            break;
        default:
            break;
        }
    }

    if (showSkeleton)
    {
        DrawSkeleton(
            ghostAI,
            projection,
            CONFIG_COLOR(GetConfigManager(), "SkeletonColor"),
            CONFIG_FLOAT(GetConfigManager(), "SkeletonThickness"));
    }

    if (showPhoto)
        DrawPhotoESP(body);
}

void GhostESP::SetD3D11Device(ID3D11Device* device)
{
    g_pd3dDevice = device;

    if (g_pd3dDevice)
        LOG_INFO("D3D11 device successfully set for Photo ESP");
    else
        LOG_ERROR("Failed to set D3D11 device - device pointer is null");
}

void GhostESP::LoadAvailablePhotos()
{
    availablePhotos.clear();

    const std::string imageDir =
        Utils::GetPhasmoCheatVDirectory() + "/Images";

    if (!std::filesystem::exists(imageDir))
    {
        LOG_WARN("Images directory does not exist: %s", imageDir.c_str());
        return;
    }

    for (const auto& entry : std::filesystem::directory_iterator(imageDir))
    {
        if (!entry.is_regular_file())
            continue;

        const auto extension = entry.path().extension();
        if (extension == ".png" || extension == ".jpg" ||
            extension == ".jpeg" || extension == ".bmp" ||
            extension == ".tga")
        {
            availablePhotos.push_back(entry.path().filename().string());
        }
    }

    LOG_INFO("Loaded %d available photos", availablePhotos.size());
}

bool GhostESP::LoadTexture(const std::string& filename, ID3D11ShaderResourceView** outSrv, int* outWidth, int* outHeight)
{
    if (!outSrv || !outWidth || !outHeight)
        return false;

    *outSrv = nullptr;
    *outWidth = 0;
    *outHeight = 0;

    if (const auto it = loadedTextures.find(filename); it != loadedTextures.end())
    {
        *outSrv = it->second.SRV;
        *outWidth = it->second.Width;
        *outHeight = it->second.Height;
        return *outSrv != nullptr;
    }

    if (!g_pd3dDevice)
    {
        LOG_ERROR("D3D11 device is not initialized. Call SetD3D11Device first.");
        return false;
    }

    const std::string fullPath =
        Utils::GetPhasmoCheatVDirectory() + "/Images/" + filename;

    int width = 0;
    int height = 0;
    int channels = 0;
    unsigned char* imageData =
        stbi_load(fullPath.c_str(), &width, &height, &channels, 4);

    if (!imageData)
    {
        LOG_ERROR("Failed to load image: %s", filename.c_str());
        return false;
    }

    D3D11_TEXTURE2D_DESC desc{};
    desc.Width = static_cast<UINT>(width);
    desc.Height = static_cast<UINT>(height);
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

    D3D11_SUBRESOURCE_DATA subResource{};
    subResource.pSysMem = imageData;
    subResource.SysMemPitch = static_cast<UINT>(width * 4);

    ID3D11Texture2D* texture = nullptr;
    const HRESULT textureResult =
        g_pd3dDevice->CreateTexture2D(&desc, &subResource, &texture);

    if (FAILED(textureResult) || !texture)
    {
        LOG_ERROR("Failed to create D3D11 texture: 0x%08X", textureResult);
        stbi_image_free(imageData);
        return false;
    }

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    srvDesc.Format = desc.Format;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;
    srvDesc.Texture2D.MostDetailedMip = 0;

    const HRESULT srvResult =
        g_pd3dDevice->CreateShaderResourceView(texture, &srvDesc, outSrv);

    texture->Release();
    stbi_image_free(imageData);

    if (FAILED(srvResult) || !*outSrv)
    {
        LOG_ERROR("Failed to create D3D11 shader resource view: 0x%08X", srvResult);
        return false;
    }

    loadedTextures.emplace(filename, TextureData{ *outSrv, width, height });
    *outWidth = width;
    *outHeight = height;
    return true;
}

void GhostESP::DrawPhotoESP(const BodySnapshot& body)
{
    if (!body.Valid)
        return;

    const int photoType = CONFIG_INT(GetConfigManager(), "PhotoESPType");
    if (photoType < 0 ||
        static_cast<size_t>(photoType) >= availablePhotos.size())
    {
        return;
    }

    ID3D11ShaderResourceView* textureSrv = nullptr;
    int textureWidth = 0;
    int textureHeight = 0;

    const std::string& selectedPhoto =
        availablePhotos[static_cast<size_t>(photoType)];

    if (!LoadTexture(selectedPhoto, &textureSrv, &textureWidth, &textureHeight) ||
        !textureSrv)
    {
        return;
    }

    float height = std::fabs(body.TopScreen.Y - body.BottomScreen.Y);
    float width = height * 0.5f;

    width *= CONFIG_FLOAT(GetConfigManager(), "PhotoWidth");
    height *= CONFIG_FLOAT(GetConfigManager(), "PhotoHeight");

    const float left = body.BottomScreen.X - width * 0.5f;
    const float right = body.BottomScreen.X + width * 0.5f;
    const float top = body.BottomScreen.Y - height;
    const float bottom = body.BottomScreen.Y;

    ImGui::GetBackgroundDrawList()->AddImage(
        textureSrv,
        ImVec2(left, top),
        ImVec2(right, bottom),
        ImVec2(0.0f, 0.0f),
        ImVec2(1.0f, 1.0f),
        IM_COL32_WHITE);
}

void GhostESP::CleanupTextures()
{
    for (auto& [name, texture] : loadedTextures)
    {
        if (texture.SRV)
        {
            texture.SRV->Release();
            texture.SRV = nullptr;
        }
    }

    loadedTextures.clear();
}

void GhostESP::DrawSkeleton(const SDK::GhostAI* ghostAI, const ProjectionContext& projection, const ImColor& color, float thickness)
{
    if (!ghostAI || !ghostAI->Fields.currentModel)
        return;

    auto* animator = ghostAI->Fields.currentModel->Fields.anim;
    if (!animator)
        return;

    BoneScreenCache screenBones{};

    for (const Bone bone : kMainBones)
    {
        const auto index = BoneIndex(bone);
        if (index >= screenBones.size())
            continue;

        auto* transform =
            SDK::Animator_GetBoneTransform(
                animator,
                bone,
                nullptr);

        if (!transform)
            continue;

        auto& cachedBone = screenBones[index];
        const SDK::Vector3 worldPos = Utils::GetPosVec3(transform);

        cachedBone.Visible =
            ProjectWorldToScreen(projection, worldPos, cachedBone.Screen) &&
            cachedBone.Screen.Z > 0.0f;
    }

    auto* drawList = ImGui::GetBackgroundDrawList();

    for (const auto& [fromBone, toBone] : kMainConnections)
    {
        const auto fromIndex = BoneIndex(fromBone);
        const auto toIndex = BoneIndex(toBone);

        if (fromIndex >= screenBones.size() || toIndex >= screenBones.size())
            continue;

        const auto& from = screenBones[fromIndex];
        const auto& to = screenBones[toIndex];

        if (!from.Visible || !to.Visible)
            continue;

        drawList->AddLine(
            ImVec2(from.Screen.X, from.Screen.Y),
            ImVec2(to.Screen.X, to.Screen.Y),
            color,
            thickness);
    }

    for (const Bone bone : kMainBones)
    {
        const auto index = BoneIndex(bone);
        if (index >= screenBones.size())
            continue;

        const auto& cachedBone = screenBones[index];
        if (!cachedBone.Visible)
            continue;

        drawList->AddCircleFilled(
            ImVec2(cachedBone.Screen.X, cachedBone.Screen.Y),
            thickness * 0.8f,
            color);
    }
}

void GhostESP::Draw2DBox(const BodySnapshot& body, const ImColor& color, float thickness)
{
    if (!body.Valid)
        return;

    const float height = std::fabs(body.TopScreen.Y - body.BottomScreen.Y);
    const float width = height * 0.5f;

    ImGui::GetBackgroundDrawList()->AddRect(
        ImVec2(body.BottomScreen.X - width * 0.5f, body.TopScreen.Y),
        ImVec2(body.BottomScreen.X + width * 0.5f, body.BottomScreen.Y),
        color,
        0.0f,
        0,
        thickness);
}

void GhostESP::DrawCornerBox(const BodySnapshot& body, const ImColor& color, float thickness)
{
    if (!body.Valid)
        return;

    const float height = std::fabs(body.TopScreen.Y - body.BottomScreen.Y);
    const float width = height * 0.5f;
    const float lineWidth = width / 3.0f;
    const float lineHeight = height / 3.0f;

    const float left = body.BottomScreen.X - width * 0.5f;
    const float right = body.BottomScreen.X + width * 0.5f;
    const float top = body.TopScreen.Y;
    const float bottom = body.BottomScreen.Y;

    auto* draw = ImGui::GetBackgroundDrawList();

    draw->AddLine(ImVec2(left, top), ImVec2(left + lineWidth, top), color, thickness);
    draw->AddLine(ImVec2(left, top), ImVec2(left, top + lineHeight), color, thickness);

    draw->AddLine(ImVec2(right, top), ImVec2(right - lineWidth, top), color, thickness);
    draw->AddLine(ImVec2(right, top), ImVec2(right, top + lineHeight), color, thickness);

    draw->AddLine(ImVec2(left, bottom), ImVec2(left + lineWidth, bottom), color, thickness);
    draw->AddLine(ImVec2(left, bottom), ImVec2(left, bottom - lineHeight), color, thickness);

    draw->AddLine(ImVec2(right, bottom), ImVec2(right - lineWidth, bottom), color, thickness);
    draw->AddLine(ImVec2(right, bottom), ImVec2(right, bottom - lineHeight), color, thickness);
}

void GhostESP::DrawFilledBox(const BodySnapshot& body, const ImColor& color, float thickness)
{
    if (!body.Valid)
        return;

    const float height = std::fabs(body.TopScreen.Y - body.BottomScreen.Y);
    const float width = height * 0.5f;

    const ImVec2 boxMin(body.BottomScreen.X - width * 0.5f,
        body.TopScreen.Y);

    const ImVec2 boxMax(body.BottomScreen.X + width * 0.5f,
        body.BottomScreen.Y);

    auto* draw = ImGui::GetBackgroundDrawList();

    draw->AddRectFilled(boxMin, boxMax,
        ImColor(color.Value.x, color.Value.y, color.Value.z, 0.2f));
    draw->AddRect(boxMin, boxMax, color, 0.0f, 0, thickness);
}

void GhostESP::DrawName(const SDK::GhostAI* ghostAI, const BodySnapshot& body, const ImColor& color)
{
    if (!body.Valid || !ghostAI || !ghostAI->Fields.GhostInfo)
        return;

    const auto& traits = ghostAI->Fields.GhostInfo->Fields.GhostTraits;
    if (!traits.Name)
        return;

    const std::string name = Utils::UnityStrToSysStr(*traits.Name);
    const std::string type = Utils::GhostEnumToStr(traits.GhostType_);
    const std::string text = type + " • " + name;

    const ImVec2 textSize = ImGui::CalcTextSize(text.c_str());

    ImGui::GetBackgroundDrawList()->AddText(
        ImVec2(
            body.BottomScreen.X - textSize.x * 0.5f,
            body.TopScreen.Y - textSize.y - 2.0f),
        color, text.c_str());
}

void GhostESP::OnMenuRender()
{
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(6, 5));

    const auto makeLabel = [](const char* code, const char* id)
        {
            return std::string(LANG(code)) + "##" + id;
        };

    bool enabled = IsActive();
    if (ImGui::Checkbox(LANG("EnableGhostESP"), &enabled))
    {
        SET_CONFIG_VALUE(GetConfigManager(), "Enabled", bool, enabled);
        enabled ? OnActivate() : OnDeactivate();
    }

    if (!enabled)
    {
        ImGui::PopStyleVar();
        return;
    }

    constexpr auto colorEditFlags = ImGuiColorEditFlags_NoInputs;

    ImColor nameColor = CONFIG_COLOR(GetConfigManager(), "NameCol");
    if (ImGui::ColorEdit4(
        makeLabel("NameColor", "ghostESP").c_str(),
        reinterpret_cast<float*>(&nameColor.Value),
        colorEditFlags))
    {
        SET_CONFIG_VALUE(GetConfigManager(), "NameCol", ImColor, nameColor);
    }

    ImGui::Separator();

    bool showBoxESP = CONFIG_BOOL(GetConfigManager(), "ShowBoxESP");
    if (ImGui::Checkbox(
        makeLabel("ShowBoxESP", "ghostESP").c_str(),
        &showBoxESP))
    {
        SET_CONFIG_VALUE(GetConfigManager(), "ShowBoxESP", bool, showBoxESP);
    }

    if (showBoxESP)
    {
        const char* boxTypes[] = {
            LANG("BoxType2D"),
            LANG("BoxTypeCorner"),
            LANG("BoxTypeFilled")
        };

        int boxType = CONFIG_INT(GetConfigManager(), "BoxType");
        if (ImGui::Combo(
            makeLabel("BoxType", "ghostESP").c_str(),
            &boxType,
            boxTypes, IM_ARRAYSIZE(boxTypes)))
        {
            SET_CONFIG_VALUE(GetConfigManager(), "BoxType", int, boxType);
        }

        ImColor espColor = CONFIG_COLOR(GetConfigManager(), "ESPColor");
        if (ImGui::ColorEdit4(
            makeLabel("Color", "ghostESP").c_str(),
            reinterpret_cast<float*>(&espColor.Value), colorEditFlags))
        {
            SET_CONFIG_VALUE(GetConfigManager(), "ESPColor", ImColor, espColor);
        }

        float boxThickness = CONFIG_FLOAT(GetConfigManager(), "BoxThickness");
        if (ImGui::SliderFloat(
            makeLabel("BoxThickness", "ghostESP").c_str(),
            &boxThickness,
            0.5f, 5.0f))
        {
            SET_CONFIG_VALUE(GetConfigManager(), "BoxThickness", float, boxThickness);
        }
    }

    ImGui::Separator();

    bool showSkeleton = CONFIG_BOOL(GetConfigManager(), "ShowSkeleton");
    if (ImGui::Checkbox(
        makeLabel("ShowSkeleton", "ghostESP").c_str(), &showSkeleton))
    {
        SET_CONFIG_VALUE(GetConfigManager(), "ShowSkeleton", bool, showSkeleton);
    }

    if (showSkeleton)
    {
        ImColor skeletonColor = CONFIG_COLOR(GetConfigManager(), "SkeletonColor");
        if (ImGui::ColorEdit4(
            makeLabel("SkeletonColor", "ghostESP").c_str(),
            reinterpret_cast<float*>(&skeletonColor.Value), colorEditFlags))
        {
            SET_CONFIG_VALUE(GetConfigManager(), "SkeletonColor", ImColor, skeletonColor);
        }

        float skeletonThickness =
            CONFIG_FLOAT(GetConfigManager(), "SkeletonThickness");
        if (ImGui::SliderFloat(
            makeLabel("SkeletonThickness", "ghostESP").c_str(),
            &skeletonThickness,
            0.5f, 3.0f))
        {
            SET_CONFIG_VALUE(
                GetConfigManager(),
                "SkeletonThickness",
                float,
                skeletonThickness);
        }
    }

    ImGui::Separator();

    bool showPhotoESP = CONFIG_BOOL(GetConfigManager(), "ShowPhotoESP");
    if (ImGui::Checkbox(
        makeLabel("ShowPhotoESP", "ghostESP").c_str(), &showPhotoESP))
    {
        SET_CONFIG_VALUE(GetConfigManager(), "ShowPhotoESP", bool, showPhotoESP);
    }

    if (showPhotoESP)
    {
        float photoWidth = CONFIG_FLOAT(GetConfigManager(), "PhotoWidth");
        if (ImGui::SliderFloat(
            makeLabel("PhotoWidth", "ghostESP").c_str(),
            &photoWidth,
            0.1f,
            3.0f, "%.2f"))
        {
            SET_CONFIG_VALUE(GetConfigManager(), "PhotoWidth", float, photoWidth);
        }

        float photoHeight = CONFIG_FLOAT(GetConfigManager(), "PhotoHeight");
        if (ImGui::SliderFloat(
            makeLabel("PhotoHeight", "ghostESP").c_str(),
            &photoHeight,
            0.1f,
            3.0f, "%.2f"))
        {
            SET_CONFIG_VALUE(GetConfigManager(), "PhotoHeight", float, photoHeight);
        }

        if (!availablePhotos.empty())
        {
            std::vector<const char*> photoNames;
            photoNames.reserve(availablePhotos.size());

            for (const auto& photo : availablePhotos)
                photoNames.push_back(photo.c_str());

            int photoType = CONFIG_INT(GetConfigManager(), "PhotoESPType");
            if (ImGui::Combo(
                makeLabel("PhotoType", "ghostESP").c_str(),
                &photoType,
                photoNames.data(),
                static_cast<int>(photoNames.size())))
            {
                SET_CONFIG_VALUE(GetConfigManager(), "PhotoESPType", int, photoType);
            }
        }

        if (ImGui::Button(makeLabel("RefreshImages", "ghostESP").c_str(), ImVec2(140, 24)))
            LoadAvailablePhotos();
    }

    ImGui::PopStyleVar();
}
