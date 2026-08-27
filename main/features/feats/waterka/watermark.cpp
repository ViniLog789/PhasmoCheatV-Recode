#include "watermark.h"
#include <windows.h>
#include <psapi.h>

using namespace PhasmoCheatV::Features::Visuals;

namespace
{
    int GetSystemCPUUsage()
    {
        static FILETIME prevIdleTime = {}, prevKernelTime = {}, prevUserTime = {};
        static bool initialized = false;

        FILETIME idleTime, kernelTime, userTime;
        if (!GetSystemTimes(&idleTime, &kernelTime, &userTime))
            return 0;

        if (!initialized)
        {
            prevIdleTime = idleTime;
            prevKernelTime = kernelTime;
            prevUserTime = userTime;
            initialized = true;
            return 0;
        }

        ULARGE_INTEGER idle, kernel, user;
        ULARGE_INTEGER prevIdle, prevKernel, prevUser;

        idle.LowPart = idleTime.dwLowDateTime;
        idle.HighPart = idleTime.dwHighDateTime;
        kernel.LowPart = kernelTime.dwLowDateTime;
        kernel.HighPart = kernelTime.dwHighDateTime;
        user.LowPart = userTime.dwLowDateTime;
        user.HighPart = userTime.dwHighDateTime;

        prevIdle.LowPart = prevIdleTime.dwLowDateTime;
        prevIdle.HighPart = prevIdleTime.dwHighDateTime;
        prevKernel.LowPart = prevKernelTime.dwLowDateTime;
        prevKernel.HighPart = prevKernelTime.dwHighDateTime;
        prevUser.LowPart = prevUserTime.dwLowDateTime;
        prevUser.HighPart = prevUserTime.dwHighDateTime;

        ULONGLONG idleDiff = idle.QuadPart - prevIdle.QuadPart;
        ULONGLONG kernelDiff = kernel.QuadPart - prevKernel.QuadPart;
        ULONGLONG userDiff = user.QuadPart - prevUser.QuadPart;
        ULONGLONG total = kernelDiff + userDiff;

        prevIdleTime = idleTime;
        prevKernelTime = kernelTime;
        prevUserTime = userTime;

        if (total == 0)
            return 0;

        return static_cast<int>(100.0 - ((idleDiff * 100.0) / total));
    }

    int GetRAMUsage()
    {
        MEMORYSTATUSEX memInfo;
        memInfo.dwLength = sizeof(MEMORYSTATUSEX);
        GlobalMemoryStatusEx(&memInfo);
        return static_cast<int>(memInfo.dwMemoryLoad);
    }

    SIZE_T GetUsedRAMMB()
    {
        MEMORYSTATUSEX memInfo;
        memInfo.dwLength = sizeof(MEMORYSTATUSEX);
        GlobalMemoryStatusEx(&memInfo);
        return (memInfo.ullTotalPhys - memInfo.ullAvailPhys) / (1024 * 1024);
    }

    SIZE_T GetTotalRAMMB()
    {
        MEMORYSTATUSEX memInfo;
        memInfo.dwLength = sizeof(MEMORYSTATUSEX);
        GlobalMemoryStatusEx(&memInfo);
        return memInfo.ullTotalPhys / (1024 * 1024);
    }
}

Watermark::Watermark() : FeatureCore(LANG("Watermark_Header"), TYPE_VISUALS)
{
    DECLARE_CONFIG(GetConfigManager(), "ShowSanity", bool, true);
    DECLARE_CONFIG(GetConfigManager(), "ShowPing", bool, true);
    DECLARE_CONFIG(GetConfigManager(), "ShowFPS", bool, true);
    DECLARE_CONFIG(GetConfigManager(), "ShowCPU", bool, true);
    DECLARE_CONFIG(GetConfigManager(), "ShowRAM", bool, true);
    DECLARE_CONFIG(GetConfigManager(), "ShowRAMMB", bool, false);
    DECLARE_CONFIG(GetConfigManager(), "ShowTime", bool, false);
    DECLARE_CONFIG(GetConfigManager(), "Position", int, 1);

    CachedText.reserve(256);
    lastInfoUpdate = 0.0f;
    cachedCPU = 0;
    cachedRAM = 0;
    cachedUsedRAM = 0;
    cachedTotalRAM = 0;
}

void Watermark::OnRender()
{
    if (!IsActive()) return;

    const bool showPing = CONFIG_BOOL(GetConfigManager(), "ShowPing");
    const bool showSanity = CONFIG_BOOL(GetConfigManager(), "ShowSanity");
    const bool showFPS = CONFIG_BOOL(GetConfigManager(), "ShowFPS");
    const bool showCPU = CONFIG_BOOL(GetConfigManager(), "ShowCPU");
    const bool showRAM = CONFIG_BOOL(GetConfigManager(), "ShowRAM");
    const bool showRAMMB = CONFIG_BOOL(GetConfigManager(), "ShowRAMMB");
    const bool showTime = CONFIG_BOOL(GetConfigManager(), "ShowTime");
    const int position = CONFIG_INT(GetConfigManager(), "Position");

    float currentTime = ImGui::GetTime();
    if (currentTime - lastInfoUpdate > 0.5f)
    {
        if (showCPU)
            cachedCPU = GetSystemCPUUsage();
        if (showRAM || showRAMMB)
        {
            cachedRAM = GetRAMUsage();
            cachedUsedRAM = GetUsedRAMMB();
            cachedTotalRAM = GetTotalRAMMB();
        }
        lastInfoUpdate = currentTime;
    }

    CachedText = Globals::IsDebugging ? "PhasmoCheatV [DEBUG]" : "PhasmoCheatV";

    if (showFPS)
    {
        const int fps = static_cast<int>(ImGui::GetIO().Framerate);
        CachedText.append(" | ");
        CachedText.append(std::to_string(fps));
        CachedText.append(" FPS");
    }

    if (showCPU)
    {
        CachedText.append(" | CPU ");
        CachedText.append(std::to_string(cachedCPU));
        CachedText.append("%");
    }

    if (showRAM)
    {
        CachedText.append(" | RAM ");
        CachedText.append(std::to_string(cachedRAM));
        CachedText.append("%");
    }

    if (showRAMMB)
    {
        CachedText.append(" | ");
        CachedText.append(std::to_string(cachedUsedRAM));
        CachedText.append("/");
        CachedText.append(std::to_string(cachedTotalRAM));
        CachedText.append(" MB");
    }

    if (showTime)
    {
        SYSTEMTIME st;
        GetLocalTime(&st);
        char timeBuf[16];
        sprintf_s(timeBuf, "%02d:%02d:%02d", st.wHour, st.wMinute, st.wSecond);
        CachedText.append(" | ");
        CachedText.append(timeBuf);
    }

    if (SDK::Application_get_isPlaying(nullptr) && Utils::GetLocalPlayer())
    {
        if (showPing && SDK::PhotonNetwork_Get_IsConnected(nullptr))
        {
            const int ping = SDK::PhotonNetwork_GetPing(nullptr);
            CachedText.append(" | ");
            CachedText.append(std::to_string(ping));
            CachedText.append(" ms");
        }
    }

    if (showSanity && SDK::MapController_sFields->instance && SDK::MapController_sFields->instance->Fields.GameController)
    {
        const int sanity = static_cast<int>(100.f - SDK::GameController_GetAveragePlayerInsanity(SDK::MapController_sFields->instance->Fields.GameController, nullptr));
        CachedText.append(" | ");
        CachedText.append(std::to_string(sanity));
        CachedText.append("% Sanity");
    }

    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[4]);
    const ImVec2 textSize = ImGui::CalcTextSize(CachedText.c_str());
    ImGui::PopFont();

    const float width = textSize.x + 24.0f;
    const float height = textSize.y + 10.0f;

    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    const float padding = 10.0f;
    ImVec2 pos;
    bool isFree = (position == 9);

    if (!isFree)
    {
        switch (position)
        {
        case 0:
            pos = ImVec2(padding, padding);
            break;
        case 1:
            pos = ImVec2(viewport->Size.x - padding - width, padding);
            break;
        case 2:
            pos = ImVec2(padding, viewport->Size.y - padding - height);
            break;
        case 3:
            pos = ImVec2(viewport->Size.x - padding - width, viewport->Size.y - padding - height);
            break;
        case 4:
            pos = ImVec2((viewport->Size.x - width) * 0.5f, padding);
            break;
        case 5:
            pos = ImVec2((viewport->Size.x - width) * 0.5f, viewport->Size.y - padding - height);
            break;
        case 6:
            pos = ImVec2(padding, (viewport->Size.y - height) * 0.5f);
            break;
        case 7:
            pos = ImVec2(viewport->Size.x - padding - width, (viewport->Size.y - height) * 0.5f);
            break;
        case 8:
            pos = ImVec2((viewport->Size.x - width) * 0.5f, (viewport->Size.y - height) * 0.5f);
            break;
        default:
            pos = ImVec2(viewport->Size.x - padding - width, padding);
            break;
        }
        ImGui::SetNextWindowPos(pos, ImGuiCond_Always);
    }
    else
    {
        ImGui::SetNextWindowPos(ImVec2(viewport->Size.x - padding - width, padding), ImGuiCond_FirstUseEver);
    }

    ImGui::SetNextWindowSize(ImVec2(width, height));

    ImGuiWindowFlags flags = Globals::WINDOW_FLAGS_GLOBALS;
    if (isFree)
        flags &= ~ImGuiWindowFlags_NoMove;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));

    ImGui::Begin("###WatermarkWindow", nullptr, flags);
    {
        ImVec2 windowPos = ImGui::GetWindowPos();
        ImVec2 windowSize = ImGui::GetWindowSize();

        ImDrawList* drawList = ImGui::GetWindowDrawList();

        drawList->AddRectFilled(
            windowPos,
            ImVec2(windowPos.x + windowSize.x, windowPos.y + windowSize.y),
            IM_COL32(15, 15, 15, 180),
            12.0f
        );

        drawList->AddRect(
            windowPos,
            ImVec2(windowPos.x + windowSize.x, windowPos.y + windowSize.y),
            IM_COL32(80, 50, 120, 200),
            12.0f,
            0,
            1.5f
        );

        const ImVec2 textPos = ImVec2(
            windowPos.x + (windowSize.x - textSize.x) / 2,
            windowPos.y + (windowSize.y - textSize.y) / 2
        );

        ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[4]);
        drawList->AddText(textPos, IM_COL32(255, 255, 255, 255), CachedText.c_str());
        ImGui::PopFont();
    }
    ImGui::End();

    ImGui::PopStyleColor();
    ImGui::PopStyleVar(4);
}

void Watermark::OnMenuRender()
{
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 6));

    bool enabled = IsActive();
    if (ImGui::Checkbox(LANG("EnableWatermark"), &enabled))
    {
        SET_CONFIG_VALUE(GetConfigManager(), "Enabled", bool, enabled);
        if (enabled) OnActivate();
        else OnDeactivate();
    }

    if (enabled)
    {
        bool showFPS = CONFIG_BOOL(GetConfigManager(), "ShowFPS");
        bool showSanity = CONFIG_BOOL(GetConfigManager(), "ShowSanity");
        bool showPing = CONFIG_BOOL(GetConfigManager(), "ShowPing");
        bool showCPU = CONFIG_BOOL(GetConfigManager(), "ShowCPU");
        bool showRAM = CONFIG_BOOL(GetConfigManager(), "ShowRAM");
        bool showRAMMB = CONFIG_BOOL(GetConfigManager(), "ShowRAMMB");
        bool showTime = CONFIG_BOOL(GetConfigManager(), "ShowTime");
        int position = CONFIG_INT(GetConfigManager(), "Position");

        if (ImGui::Checkbox(LANG("Watermark_ShowFPS"), &showFPS))
            SET_CONFIG_VALUE(GetConfigManager(), "ShowFPS", bool, showFPS);

        if (ImGui::Checkbox(LANG("Watermark_ShowSanity"), &showSanity))
            SET_CONFIG_VALUE(GetConfigManager(), "ShowSanity", bool, showSanity);

        if (ImGui::Checkbox(LANG("Watermark_ShowPing"), &showPing))
            SET_CONFIG_VALUE(GetConfigManager(), "ShowPing", bool, showPing);

        if (ImGui::Checkbox(LANG("Watermark_ShowCPU"), &showCPU))
            SET_CONFIG_VALUE(GetConfigManager(), "ShowCPU", bool, showCPU);

        if (ImGui::Checkbox(LANG("Watermark_ShowRAM"), &showRAM))
            SET_CONFIG_VALUE(GetConfigManager(), "ShowRAM", bool, showRAM);

        if (ImGui::Checkbox(LANG("Watermark_ShowRAMMB"), &showRAMMB))
            SET_CONFIG_VALUE(GetConfigManager(), "ShowRAMMB", bool, showRAMMB);

        if (ImGui::Checkbox(LANG("Watermark_ShowTime"), &showTime))
            SET_CONFIG_VALUE(GetConfigManager(), "ShowTime", bool, showTime);

        const char* positions[] = {
            LANG("Watermark_TopLeft"),
            LANG("Watermark_TopRight"),
            LANG("Watermark_BottomLeft"),
            LANG("Watermark_BottomRight"),
            LANG("Watermark_TopCenter"),
            LANG("Watermark_BottomCenter"),
            LANG("Watermark_MiddleLeft"),
            LANG("Watermark_MiddleRight"),
            LANG("Watermark_Center"),
            LANG("Watermark_Free")
        };

        if (ImGui::Combo(LANG("Watermark_Position"), &position, positions, IM_ARRAYSIZE(positions)))
            SET_CONFIG_VALUE(GetConfigManager(), "Position", int, position);
    }

    ImGui::PopStyleVar();
}