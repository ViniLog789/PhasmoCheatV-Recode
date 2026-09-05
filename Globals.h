#pragma once
#include <atomic>
#include <Windows.h>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"
#define COSMETICSUNLOCKER 0 //! dont change this if u not have need files

namespace PhasmoCheatV::Globals
{
	// DEBUG
	inline bool IsDebugging = false;
	inline bool IsCalledLogs = false;
	inline bool IsUpdateCalledLogs = false; //! a lot of spam
	inline bool RedirectUnityLogs = false; //! a lot of spam
	inline bool IsRPCLogs = false;
	inline std::chrono::steady_clock::time_point CheatInjectedTime = std::chrono::steady_clock::now();
	inline bool ForTestsFlag = false;

	// MAIN
	inline HMODULE globalModule{};
	inline std::atomic<bool> CheatWork{ false };
	inline bool isHunting = false;
	inline int MenuToggleKey = VK_RETURN;
	inline bool IsBeta = true;
	inline std::string Version = "3.1";
	inline std::string betaVersion = "3.1.2";
	inline char statusRPC[128] = "Waiting...";

	// IMGUI
	inline float dpiScale = GetDpiForSystem() / 96.0f;
	inline constexpr ImGuiWindowFlags WINDOW_FLAGS_GLOBALS =
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_AlwaysAutoResize |
		ImGuiWindowFlags_NoResize;
	inline ImVec4 accentPurple = ImVec4(0.51f, 0.25f, 0.96f, 1.00f);
	inline ImVec4 darkerBg = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
	inline ImVec4 accentPurpleDark = ImVec4(0.41f, 0.15f, 0.86f, 1.00f);
	inline ImVec4 accentPurpleLight = ImVec4(0.61f, 0.35f, 1.00f, 1.00f);
	inline ImVec4 darkBg = ImVec4(0.08f, 0.08f, 0.08f, 0.98f);
	inline ImVec4 cardBg = ImVec4(0.12f, 0.12f, 0.14f, 1.00f);
	inline ImVec4 headerBg = ImVec4(0.10f, 0.10f, 0.12f, 1.00f);
	inline bool IsMenuBlur = false;
	inline bool IsMenuRain = true;
	inline bool IsMenuFireflies = false;
	inline bool IsMenuStars = true;
	inline bool IsMenuPetals = false;
	inline bool DiscordRPC = false;
	inline bool RPCShowName = false;
	inline ImFont* g_FontMain = nullptr;
	inline ImFont* g_FontChinese = nullptr;
	inline ImFont* g_FontActive = nullptr;
	inline bool g_FontChineseReady = false;

	// FUNCTIONS
	inline std::string GetVersion()
	{
		return IsBeta ? betaVersion : Version;
	}

	inline void ResetColors()
	{
		accentPurple = ImVec4(0.51f, 0.25f, 0.96f, 1.00f);
		darkerBg = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
		accentPurpleDark = ImVec4(0.41f, 0.15f, 0.86f, 1.00f);
		accentPurpleLight = ImVec4(0.61f, 0.35f, 1.00f, 1.00f);
		darkBg = ImVec4(0.08f, 0.08f, 0.08f, 0.98f);
		cardBg = ImVec4(0.12f, 0.12f, 0.14f, 1.00f);
		headerBg = ImVec4(0.10f, 0.10f, 0.12f, 1.00f);
	}

	inline bool IsProxyMode(HMODULE hModule)
	{
		char path[MAX_PATH];
		GetModuleFileNameA(hModule, path, MAX_PATH);

		std::string file = path;
		size_t pos = file.find_last_of("\\/");
		if (pos != std::string::npos)
			file = file.substr(pos + 1);

		return _stricmp(file.c_str(), "d3d11.dll") == 0;
	}
}
