#include "fullbright.h"

using namespace PhasmoCheatV::Features::Visuals;

Fullbright::Fullbright() : FeatureCore(LANG("Fullbright_Header"), TYPE_VISUALS)
{
    DECLARE_CONFIG(GetConfigManager(), "Intensity", float, 0.3f);
    DECLARE_CONFIG(GetConfigManager(), "Range", float, 15.f);
}

static bool IsBleasdaleFarmhouse()
{
    auto* map = Utils::GetMap();
    if (!map || !map->Fields.mapName) return false;
    std::string name = Utils::UnityStrToSysStr(*map->Fields.mapName);
    return name.find("Bleasdale") != std::string::npos;
}

void Fullbright::OnMenuRender()
{
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 6));

    bool enabled = IsActive();
    if (BCheckBox(LANG("EnableFullbright"), &enabled, "b_EnableFullbright"))
    {
        SET_CONFIG_VALUE(GetConfigManager(), "Enabled", bool, enabled);
        if (enabled) OnActivate();
        else OnDeactivate();
    }

    if (enabled) {
        float intensity = CONFIG_FLOAT(GetConfigManager(), "Intensity");
        float range = CONFIG_FLOAT(GetConfigManager(), "Range");

        if (ImGui::SliderFloat("##Intensity", &intensity, 0.1f, 10.f, (std::string("%.1f ") + LANG("LightIntensity")).c_str()))
            SET_CONFIG_VALUE(GetConfigManager(), "Intensity", float, intensity);

        if (ImGui::SliderFloat("##Range", &range, 1.f, 100.f, (std::string("%.0f ") + LANG("LightRange")).c_str()))
            SET_CONFIG_VALUE(GetConfigManager(), "Range", float, range);

        if (ImGui::Button(LANG("ForceApply")))
        {
            BoostDarkMaterials();
            FullbrightMain();
        }
    }

    ImGui::PopStyleVar();
}

static void SetShaderKeywords(bool disable)
{
    auto setKwd = [&](const char* kw) {
        auto* str = Utils::SysStrToUnityStr(kw);
        if (str) {
            if (disable) SDK::Shader_DisableKeyword(str, nullptr);
            else        SDK::Shader_EnableKeyword(str, nullptr);
        }
    };
    setKwd("LIGHTMAP_ON");
    setKwd("LIGHTMAP_SHADOW_MIXING");
    setKwd("DIRLIGHTMAP_COMBINED");
    setKwd("SHADOWS_SHADOWMASK");
}

// run on every frame, only if Bleasdale map is detected
void Fullbright::BoostPlayerLights()
{
    float intensity = CONFIG_FLOAT(GetConfigManager(), "Intensity");
    float range = CONFIG_FLOAT(GetConfigManager(), "Range");

    if (!InGame::firstPersonController) return;
    auto* camera = InGame::firstPersonController->Fields.Camera;
    if (!camera) return;

    auto* camTransform = SDK::Component_Get_Transform((SDK::Component*)camera, nullptr);
    if (!camTransform) return;
    SDK::Vector3 camPos = SDK::Transform_Get_Position(camTransform, nullptr);

    auto* lightType = Utils::GetType("UnityEngine.Light");
    if (!lightType) return;

    auto* arr = SDK::Object_FindObjectsOfType(lightType, nullptr);
    if (!arr || arr->MaxLength == 0) return;

    for (uint32_t i = 0; i < arr->MaxLength; i++) {
        auto* lightObj = arr->Vector[i];
        if (!lightObj) continue;

        auto* lightGO = SDK::Component_Get_GameObject((SDK::Component*)lightObj, nullptr);
        if (!lightGO) continue;

        auto* lightTransform = SDK::GameObject_get_transform(lightGO, nullptr);
        if (!lightTransform) continue;

        SDK::Vector3 lightPos = SDK::Transform_Get_Position(lightTransform, nullptr);
        float dx = lightPos.X - camPos.X;
        float dy = lightPos.Y - camPos.Y;
        float dz = lightPos.Z - camPos.Z;

        if (dx * dx + dy * dy + dz * dz <= 25.0f) {
            auto* light = (SDK::Light*)lightObj;
            SDK::Light_intensity_set(light, intensity * 2.0f, nullptr);
            SDK::Light_range_set(light, range * 1.5f, nullptr);
            SDK::Light_shadows_set(light, SDK::ShadowsType::None, nullptr);
            SDK::Light_renderMode_set(light, SDK::RenderMode::ForcePixel, nullptr);
        }
    }
}

void Fullbright::BoostDarkMaterials()
{
    SetShaderKeywords(true);
    SDK::RenderSettings_set_fog(false, nullptr);
    SDK::Color white = { 1, 1, 1, 1 };
    SDK::RenderSettings_set_ambientMode(0, nullptr);
    SDK::RenderSettings_set_ambientIntensity(2.0f, nullptr);
    SDK::RenderSettings_set_ambientLight(white, nullptr);
    SDK::RenderSettings_set_ambientSkyColor(white, nullptr);
    SDK::RenderSettings_set_ambientEquatorColor(white, nullptr);
    SDK::RenderSettings_set_ambientGroundColor(white, nullptr);

    auto* rendererType = Utils::GetType("UnityEngine.Renderer");
    if (!rendererType) return;

    auto* arr = SDK::Object_FindObjectsOfType(rendererType, nullptr);
    if (!arr || arr->MaxLength == 0) return;

    constexpr float kDarkThreshold = 0.15f;
    constexpr float kTargetBrightness = 0.28f;
    constexpr float kEmissionThreshold = 0.06f;

    auto* baseColorStr = Utils::SysStrToUnityStr("_BaseColor");
    auto* colorStr = Utils::SysStrToUnityStr("_Color");
    auto* occStr = Utils::SysStrToUnityStr("_OcclusionStrength");
    auto* emissionStr = Utils::SysStrToUnityStr("_EmissionColor");
    auto* emissionKwStr = Utils::SysStrToUnityStr("_EMISSION");

    if (!baseColorStr || !colorStr || !occStr || !emissionStr || !emissionKwStr) return;

    auto* unlitShaderName = Utils::SysStrToUnityStr("Universal Render Pipeline/Unlit");
    SDK::Shader* unlitShader = nullptr;
    if (unlitShaderName)
        unlitShader = SDK::Shader_Find(unlitShaderName, nullptr);

    for (uint32_t i = 0; i < arr->MaxLength; i++)
    {
        auto* obj = arr->Vector[i];
        if (!obj) continue;

        auto* renderer = reinterpret_cast<SDK::Render*>(obj);
        auto* mat = SDK::Render_get_sharedMaterial(renderer, nullptr);
        if (!mat) continue;

        SDK::Color baseColor = SDK::Material_GetColor(mat, baseColorStr, nullptr);
        float brightness = baseColor.R * 0.2126f + baseColor.G * 0.7152f + baseColor.B * 0.0722f;

        if (brightness <= 0.0f) {
            baseColor = SDK::Material_get_color(mat, nullptr);
            brightness = baseColor.R * 0.2126f + baseColor.G * 0.7152f + baseColor.B * 0.0722f;
        }

        if (brightness <= 0.0f || brightness >= kDarkThreshold)
            continue;

        if (brightness < kEmissionThreshold && unlitShader) {
            SDK::Material_set_shader(mat, unlitShader, nullptr);
            SDK::Color mildGrey = { 0.25f, 0.25f, 0.25f, 1.0f };
            SDK::Material_set_color(mat, mildGrey, nullptr);
            SDK::Material_SetColor(mat, baseColorStr, mildGrey, nullptr);
        }
        else {
            float scale = kTargetBrightness / brightness;
            SDK::Color boosted = {
                (std::min)(baseColor.R * scale, 0.45f),
                (std::min)(baseColor.G * scale, 0.45f),
                (std::min)(baseColor.B * scale, 0.45f),
                baseColor.A
            };
            SDK::Material_SetColor(mat, baseColorStr, boosted, nullptr);
            SDK::Material_SetColor(mat, colorStr, boosted, nullptr);
            SDK::Material_SetFloat(mat, occStr, 0.0f, nullptr);
            SDK::Color lowEmission = { 0.06f, 0.06f, 0.06f, 1.0f };
            SDK::Material_SetColor(mat, emissionStr, lowEmission, nullptr);
            SDK::Material_EnableKeyword(mat, emissionKwStr, nullptr);
        }

        SDK::Render_set_lightmapIndex(renderer, 0xFFFF, nullptr);
        SDK::Vector4 zeroOffset = { 0, 0, 0, 0 };
        SDK::Render_set_lightmapScaleOffset(renderer, zeroOffset, nullptr);
    }
}

void Fullbright::OnDeactivate()
{
    if (m_lightGO) {
        SDK::Object_Destroy((SDK::Object*)m_lightGO, nullptr);
        m_lightGO = nullptr;
        m_lightComp = nullptr;
    }
    else {
        if (m_fbNameStr) {
            auto* go = SDK::GameObject_Find(m_fbNameStr, nullptr);
            if (go) {
                SDK::Object_Destroy((SDK::Object*)go, nullptr);
            }
        }
    }

    if (m_bleasdaleInitDone) {
        SetShaderKeywords(false);
        SDK::RenderSettings_set_fog(true, nullptr);
        SDK::RenderSettings_set_ambientMode(1, nullptr);
        SDK::RenderSettings_set_ambientIntensity(1.0f, nullptr);
        m_bleasdaleInitDone = false;
    }
}

void Fullbright::OnActivate()
{
    if (!m_fbNameStr) {
        m_fbNameStr = Utils::SysStrToUnityStr("VComTeamLight");
    }

    m_lightGO = SDK::GameObject_Find(m_fbNameStr, nullptr);

    if (m_lightGO) {
        if (!m_cachedLightType) {
            auto typeName = Utils::SysStrToUnityStr("UnityEngine.Light, UnityEngine.CoreModule");
            m_cachedLightType = SDK::System_Type_GetType(typeName, nullptr);
        }
        m_lightComp = (SDK::Light*)SDK::GameObject_GetComponent(m_lightGO, m_cachedLightType, nullptr);
    }
}

void Fullbright::FullbrightMain()
{
    if (!IsActive()) {
        if (m_lightGO) {
            SDK::GameObject_SetActive(m_lightGO, false, nullptr);
        }
        return;
    }

    auto localPlayer = Utils::GetLocalPlayer();
    if (!localPlayer) return;

    if (!m_cachedLightType) {
        auto typeName = Utils::SysStrToUnityStr("UnityEngine.Light, UnityEngine.CoreModule");
        m_cachedLightType = SDK::System_Type_GetType(typeName, nullptr);
    }

    if (!m_lightGO) {
        m_lightGO = (SDK::GameObject*)CreateIl2CppObject("UnityEngine.CoreModule", "UnityEngine", "GameObject");
        if (!m_lightGO) return;

        SDK::Object_set_name((SDK::Object*)m_lightGO, m_fbNameStr, nullptr);

        m_lightComp = (SDK::Light*)SDK::GameObject_AddComponent(m_lightGO, m_cachedLightType, nullptr);
        if (!m_lightComp) {
            m_lightGO = nullptr;
            return;
        }

        SDK::Light_type_set(m_lightComp, SDK::LightType::Point, nullptr);
        SDK::Light_shadows_set(m_lightComp, SDK::ShadowsType::None, nullptr);
        SDK::Light_renderMode_set(m_lightComp, SDK::RenderMode::ForceVertex, nullptr);

        SDK::Object_DontDestroyOnLoad((SDK::Object*)m_lightGO, nullptr);
        SDK::GameObject_SetActive(m_lightGO, true, nullptr);
    }

    float intensity = CONFIG_FLOAT(GetConfigManager(), "Intensity");
    float range = CONFIG_FLOAT(GetConfigManager(), "Range");

    SDK::Light_intensity_set(m_lightComp, intensity, nullptr);
    SDK::Light_range_set(m_lightComp, range, nullptr);

    if (!SDK::GameObject_get_activeSelf(m_lightGO, nullptr)) {
        SDK::GameObject_SetActive(m_lightGO, true, nullptr);
    }

    auto lightTransform = SDK::GameObject_get_transform(m_lightGO, nullptr);
    auto playerTransform = SDK::Component_Get_Transform((SDK::Component*)localPlayer, nullptr);

    if (lightTransform && playerTransform) {
        SDK::Vector3 playerPos = Utils::GetPosVec3(localPlayer);
        playerPos.Y += 3.5f;
        SDK::Transform_Set_Position(lightTransform, playerPos, nullptr);
        SDK::Transform_Set_Rotation(lightTransform, SDK::identityQuaternion, nullptr);
    }

    if (IsBleasdaleFarmhouse()) {
        if (!m_bleasdaleInitDone) {
            SetShaderKeywords(true);
            SDK::RenderSettings_set_fog(false, nullptr);
            SDK::Color white = { 1.0f, 1.0f, 1.0f, 1.0f };
            SDK::RenderSettings_set_ambientMode(0, nullptr);
            SDK::RenderSettings_set_ambientIntensity(2.0f, nullptr);
            SDK::RenderSettings_set_ambientLight(white, nullptr);
            m_bleasdaleInitDone = true;
        }
        BoostPlayerLights();
    }
    else {
        m_bleasdaleInitDone = false;
    }
}