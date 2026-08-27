#include "menu.h"
#include "styles.h"
#include <random>
#include <chrono>
#include "../features/features_includes.h"
#include "effects/directx_blur.h"
#include "effects/custom_background.h"
#include "effects/rain.h"
#include "effects/stars.h"
#include "effects/fireflies.h"
#include "effects/petals.h"
#include <filesystem>
#include <shellapi.h>
#include "../discordrpc/discordrpc.h"

inline ImVec2 operator+(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x + rhs.x, lhs.y + rhs.y); }

using namespace PhasmoCheatV; namespace fs = std::filesystem;

SDK::CursorLockMode Menu::previousCursorLockMode = SDK::CursorLockMode::None;

void Menu::Initialize() {
    ImGui::CreateContext();
    SetMenuDefaultStyle();
    InitFonts();
    dx_blur_init(renderer->Swapchain, renderer->Device, renderer->Context);
    CustomBackground::RefreshFileList();
    Initialized = true;
}

void Menu::NewYear()
{
    static std::vector<ImVec2> snow(80);
    static bool init = false;
    if (!init) {
        init = true;
        for (auto& s : snow)
            s = ImVec2((float)(rand() % (int)ImGui::GetIO().DisplaySize.x),
                (float)(rand() % (int)ImGui::GetIO().DisplaySize.y));
    }

    auto& io = ImGui::GetIO();
    ImDrawList* dl = ImGui::GetBackgroundDrawList();

    ImGui::PushFont(io.Fonts->Fonts[6]);
    ImU32 snowColor = IM_COL32(230, 230, 255, 200);
    for (auto& s : snow) {
        dl->AddText(s, snowColor, "S");
        s.y += 0.4f + (rand() % 100) / 200.f;
        if (s.y > io.DisplaySize.y)
            s = ImVec2((float)(rand() % (int)io.DisplaySize.x), -10.f);
    }
    ImGui::PopFont();
}


void Menu::Render()
{
    constexpr ImGuiWindowFlags windowFlags =
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoScrollWithMouse |
        ImGuiWindowFlags_NoBackground;

    constexpr int menuWidth = 900;
    constexpr int menuHeight = 650;
    constexpr float tabBarWidth = 200.f;

    ImGui::SetNextWindowSize(ImVec2(menuWidth * dpiScale, menuHeight * dpiScale), ImGuiCond_Once);
    ImGui::SetNextWindowBgAlpha(1.0f);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

    ImGui::Begin("Developed with love by VComDev / VCom Team##MainMenu", nullptr, windowFlags);

    ImGui::PopStyleVar();

    // migration
    {
        static bool s_migrationChecked = false;
        static bool s_needsMigration = false;
        static bool s_migrationFailed = false;

        if (!s_migrationChecked)
        {
            s_migrationChecked = true;
            s_needsMigration = Utils::NeedsMigration();
            if (s_needsMigration)
                ImGui::OpenPopup("##MigrationModal");
        }

        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
        ImGui::SetNextWindowSize(ImVec2(480 * dpiScale, 0), ImGuiCond_Always);

        constexpr ImGuiWindowFlags modalFlags =
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_AlwaysAutoResize;

        if (ImGui::BeginPopupModal("##MigrationModal", nullptr, modalFlags))
        {
            ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
            ImGui::TextColored(accentPurple, "Configuration Migration");
            ImGui::PopFont();

            ImGui::Separator();
            ImGui::Dummy(ImVec2(0, 8));

            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 6));

            ImGui::TextColored(ImVec4(0.9f, 0.9f, 1.0f, 1.0f),
                "PhasmoCheatV has moved its storage folder.");

            ImGui::Dummy(ImVec2(0, 6));

            ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.7f, 1.0f), "Old path:");
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.4f, 1.0f), "C:\\PhasmoCheatV");

            ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.7f, 1.0f), "New path:");
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(0.4f, 1.0f, 0.5f, 1.0f), "C:\\VComDev\\PhasmoCheatV");

            ImGui::Dummy(ImVec2(0, 6));

            ImGui::TextWrapped(
                "Your configs, images and fonts will be automatically copied "
                "to the new location. The old folder will be removed."
            );

            ImGui::Dummy(ImVec2(0, 4));

            if (s_migrationFailed)
            {
                ImGui::TextColored(ImVec4(1.f, 0.3f, 0.3f, 1.f),
                    "Migration failed! Check logs for details.");
                ImGui::Dummy(ImVec2(0, 4));
            }

            ImGui::TextColored(ImVec4(1.0f, 0.85f, 0.2f, 1.0f),
                "Press OK to proceed. This step cannot be skipped.");

            ImGui::Dummy(ImVec2(0, 10));
            ImGui::Separator();
            ImGui::Dummy(ImVec2(0, 6));

            float btnW = 120 * dpiScale;
            ImGui::SetCursorPosX((ImGui::GetWindowWidth() - btnW) * 0.5f);

            ImGui::PushStyleColor(ImGuiCol_Button, accentPurpleDark);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, accentPurple);
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, accentPurpleLight);

            if (ImGui::Button("Okay", ImVec2(btnW, 32 * dpiScale)))
            {
                s_migrationFailed = !Utils::MigrateDirectory();

                if (!s_migrationFailed)
                {
                    Utils::CreatePhasmoCheatVDirectory();
                    s_needsMigration = false;
                    ImGui::CloseCurrentPopup();
                }
            }

            ImGui::PopStyleVar();
            ImGui::PopStyleColor(3);

            ImGui::EndPopup();
        }
    }

    ImDrawList* dl = ImGui::GetWindowDrawList();
    ImVec2 pos = ImGui::GetWindowPos();
    ImVec2 size = ImGui::GetWindowSize();
    float  rounding = ImGui::GetStyle().WindowRounding;

    if (Globals::IsMenuBlur)
        dx_blur_draw(dl, pos, pos + size, 15.f);

    bool customBgActive = CustomBackground::Enabled
        && !CustomBackground::SelectedFile.empty();

    bool customReplaces = customBgActive
        && CustomBackground::Mode == CustomBackground::DrawMode::Replace;

    if (!Globals::IsMenuBlur && !customReplaces)
    {
        dl->AddRectFilled(pos, pos + size,
            ImGui::GetColorU32(Globals::darkBg), rounding);
    }

    CustomBackground::Draw(dl, pos, size, rounding);

    if (Globals::IsMenuRain)
        Rain();
    if (Globals::IsMenuFireflies)
        Fireflies();
    if (Globals::IsMenuStars)
        StarrySky();
    if (Globals::IsMenuPetals)
        Petals();

    ImVec2 contentSize = ImGui::GetContentRegionAvail();
    ImGui::BeginChild("MainLayout", contentSize, false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

    ImGui::BeginChild("TabBar", ImVec2(tabBarWidth * dpiScale, contentSize.y), false);
    {
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 4));

        ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
        ImGui::SetCursorPosY(20 * dpiScale);

        ImGui::SetCursorPosX((tabBarWidth * dpiScale - ImGui::CalcTextSize("PHASMOCHEATV").x) * 0.5f);
        ImGui::TextColored(accentPurple, "PHASMOCHEATV");

        ImGui::SetCursorPosX((tabBarWidth * dpiScale - ImGui::CalcTextSize("by VCom Team").x) * 0.5f);
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.8f, 0.8f), "by VCom Team");
        ImGui::PopFont();

        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10 * dpiScale);
        ImGui::Separator();
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 20 * dpiScale);

        auto DrawTabButton = [&](const char* label, int tabIndex) {
            bool isActive = menu.currentTab == tabIndex;
            ImVec2 sz(tabBarWidth * dpiScale - 40, 50 * dpiScale);
            float x = (tabBarWidth * dpiScale - sz.x) * 0.5f;
            ImGui::SetCursorPosX(x);

            ImGui::PushStyleColor(ImGuiCol_Button, isActive ? accentPurpleDark : ImVec4(0, 0, 0, 0));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, isActive ? accentPurple : ImVec4(0.15f, 0.15f, 0.15f, 1));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, accentPurpleLight);
            ImGui::PushStyleColor(ImGuiCol_Text, isActive ? ImVec4(1, 1, 1, 1) : ImVec4(0.8f, 0.8f, 0.9f, 1));

            if (ImGui::Button(label, sz)) menu.currentTab = tabIndex;

            if (isActive) {
                auto p0 = ImGui::GetItemRectMin();
                auto p1 = ImGui::GetItemRectMax();
                ImGui::GetWindowDrawList()->AddRectFilled(
                    ImVec2(p0.x, p1.y - 2), p1, ImGui::GetColorU32(accentPurpleLight)
                );
            }

            ImGui::PopStyleColor(4);
            };

        DrawTabButton(LANG("Menu_Features"), 0);
        DrawTabButton(LANG("Menu_Settings"), 1);
        DrawTabButton(LANG("Menu_About"), 2);
        DrawTabButton(LANG("Menu_Hotkeys"), 3);
        if (IsDebugging)
            DrawTabButton("TEST", 4);

        float bottomBlock = 120 * dpiScale;

        ImGui::SetCursorPosY(contentSize.y - bottomBlock);

        ImGui::Separator();
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 20 * dpiScale);

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.2f, 0.2f, 0.3f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.2f, 0.2f, 0.5f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 0.2f, 0.2f, 0.7f));
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.6f, 0.6f, 1.0f));

        ImVec2 sz(tabBarWidth * dpiScale - 40, 45 * dpiScale);
        float x = (tabBarWidth * dpiScale - sz.x) * 0.5f;
        ImGui::SetCursorPosX(x);

        if (!IsProxyMode(globalModule))
        {
            if (ImGui::Button(LANG("Menu_Unload"), sz)) {
                menu.Toggle();
                CheatWork = false;
            }
        }
        else
        {
            ImGui::Text(LANG("Proxy_Unload"));
        }

        ImGui::PopStyleColor(4);

        ImGui::SetCursorPosY(ImGui::GetWindowHeight() - 30 * dpiScale);
        ImGui::Separator();

        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2.5 * dpiScale);
        std::string versionText = Globals::GetVersion();
        if (IsBeta)
            versionText += " | BETA";
        else
            versionText += " | MIT License";

        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2.5 * dpiScale);
        ImGui::SetCursorPosX((tabBarWidth * dpiScale - ImGui::CalcTextSize(versionText.c_str()).x) * 0.5f);

        ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.6f, 0.7f), "%s", versionText.c_str());

        ImGui::PopStyleVar();
    }

    ImGui::EndChild();

    ImGui::SameLine();

    ImGui::BeginChild("ContentArea", ImVec2(contentSize.x - tabBarWidth * dpiScale, contentSize.y), false);
    {
        switch (menu.currentTab)
        {
        case 0:
            ImGui::BeginChild("FeaturesContent", ImVec2(0, 0), false);
            {
                ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10 * dpiScale);

                if (GET_FEATURE_HANDLER())
                {
                    GET_FEATURE_HANDLER()->RenderMenu();
                }
                else
                {
                    ImGui::SetCursorPos(ImVec2(
                        (ImGui::GetWindowWidth() - ImGui::CalcTextSize(LANG("Menu_NoFeatures")).x) * 0.5f,
                        (ImGui::GetWindowHeight() - ImGui::GetTextLineHeight()) * 0.5f
                    ));
                    ImGui::TextColored(ImVec4(1.0f, 0.4f, 0.4f, 1.0f), LANG("Menu_NoFeatures"));
                }
            }
            ImGui::EndChild();
            break;

        case 1:
        {
            ImGui::SetCursorPos(ImVec2(25 * dpiScale, 20 * dpiScale));

            ImGui::BeginGroup();

            ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
            ImGui::TextColored(accentPurple, LANG("Menu_Settings"));
            ImGui::PopFont();

            ImGui::Separator();
            ImGui::Dummy(ImVec2(0, 10));

            ImGui::BeginChild("GeneralSettings", ImVec2(0, 0), true);
            {
                ImGui::Columns(2, nullptr, false);
                {
                    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[7]);
                    ImGui::TextColored(ImVec4(0.8f, 0.8f, 1.f, 1.f), LANG("Menu_General"));
                    ImGui::PopFont();

                    ImGui::Spacing();

                    ImGui::Text(LANG("Menu_ToggleKey"));

                    ImGui::PushStyleColor(ImGuiCol_Button, accentPurpleDark);
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, accentPurple);

                    if (ImGui::Button(LANG("Menu_SetKey"), ImVec2(140 * dpiScale, 28 * dpiScale)))
                    {
                        ImGui::OpenPopup("Set Menu Key");
                    }

                    ImGui::PopStyleColor(2);

                    ImGui::SameLine();
                    ImGui::TextColored(accentPurpleLight, Utils::getKeyName(MenuToggleKey).c_str());

                    ImGui::Spacing();
                    if (ImGui::Button(LANG("Menu_ClearLogs"), ImVec2(160 * dpiScale, 30 * dpiScale)))
                    {
                        std::string logsPath = Utils::GetPhasmoCheatVDirectory() + "\\logs";

                        try
                        {
                            int deleted = 0;
                            for (const auto& entry : fs::directory_iterator(logsPath))
                            {
                                if (entry.is_regular_file() && fs::remove(entry.path()))
                                    deleted++;
                            }
                            LOG_INFO("Logs cleaned: ", deleted);
                        }
                        catch (...) {}
                    }

                    ImGui::SameLine();

                    if (ImGui::Button(LANG("Menu_ResetConfig")))
                        Config::ResetConfig();

                    ImGui::Spacing();
                    if (ImGui::Checkbox(LANG("Menu_DiscordRPC"), &Globals::DiscordRPC))
                    {
                        Config::SaveConfig();
                        Globals::DiscordRPC ? Discord::Initialize() : Discord::Shutdown();
                    }
                    if (Globals::DiscordRPC)
                        if (ImGui::Checkbox(LANG("Menu_RPCShowName"), &Globals::RPCShowName)) Config::SaveConfig();

                    ImGui::Spacing();
                    ImGui::Text("Language");

                    ImGui::SetNextItemWidth(120 * dpiScale);

                    static const char* langItems[] = { LANG("Menu_Language_EN"), LANG("Menu_Language_RU"), LANG("Menu_Language_CN") };
                    static int currentLang = (int)LanguageManager::GetCurrentLanguage();
                    if (ImGui::Combo("##lang", &currentLang, langItems, 3))
                    {
                        Language lang = (Language)currentLang;

                        LanguageManager::SetLanguage(lang);
                        LanguageManager::SaveLanguage();

                        ImGuiIO& io = ImGui::GetIO();

                        if (lang == Language::ZH)
                            g_FontActive = g_FontChinese;
                        else
                            g_FontActive = g_FontMain;

                        io.FontDefault = g_FontActive;
                    }

                    ImGui::Spacing();
                    CustomBackground::DrawSettingsUI(dpiScale);
                }

                ImGui::NextColumn();
                {
                    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[7]);
                    ImGui::TextColored(ImVec4(0.8f, 0.8f, 1.f, 1.f), LANG("Menu_SettText"));
                    ImGui::PopFont();

                    ImGui::Spacing();

                    bool changed = false;

                    changed |= ImGui::ColorEdit4("Accent", (float*)&Globals::accentPurple);
                    changed |= ImGui::ColorEdit4("Accent Dark", (float*)&Globals::accentPurpleDark);
                    changed |= ImGui::ColorEdit4("Accent Light", (float*)&Globals::accentPurpleLight);

                    changed |= ImGui::ColorEdit4("Dark BG", (float*)&Globals::darkBg);
                    changed |= ImGui::ColorEdit4("Darker BG", (float*)&Globals::darkerBg);
                    changed |= ImGui::ColorEdit4("Card BG", (float*)&Globals::cardBg);
                    changed |= ImGui::ColorEdit4("Header BG", (float*)&Globals::headerBg);

                    ImGui::Spacing();

                    if (ImGui::Checkbox(LANG("Menu_IsBlur"), &Globals::IsMenuBlur))
                        Config::SaveConfig();
                    ImGui::SameLine();
                    if (ImGui::Checkbox(LANG("Menu_IsRain"), &Globals::IsMenuRain))
                        Config::SaveConfig();
                    if (ImGui::Checkbox(LANG("Menu_IsFireflies"), &Globals::IsMenuFireflies))
                        Config::SaveConfig();
                    ImGui::SameLine();
                    if (ImGui::Checkbox(LANG("Menu_IsPetals"), &Globals::IsMenuPetals))
                        Config::SaveConfig();
                    if (ImGui::Checkbox(LANG("Menu_IsStars"), &Globals::IsMenuStars))
                        Config::SaveConfig();

                    ImGui::Spacing();

                    if (ImGui::Button(LANG("Menu_ApplyColors"), ImVec2(160 * dpiScale, 30 * dpiScale)))
                    {
                        Config::SaveConfig();
                        SetMenuDefaultStyle();
                    }

                    ImGui::SameLine();

                    if (ImGui::Button(LANG("Menu_ResetColors"), ImVec2(140 * dpiScale, 30 * dpiScale)))
                    {
                        Globals::ResetColors();
                        Config::SaveConfig();
                    }

                    if (changed)
                    {
                        Config::SaveConfig();
                    }
                }

                ImGui::Columns(1);
                if (ImGui::BeginPopupModal(LANG("Set Menu Key"), nullptr, ImGuiWindowFlags_AlwaysAutoResize))
                {
                    ImGui::Text(LANG("Menu_PressKey"));
                    ImGui::Separator();

                    if (ImGui::Button(LANG("Menu_Cancel")))
                        ImGui::CloseCurrentPopup();

                    for (int key = 0; key < 256; key++)
                    {
                        if (GetAsyncKeyState(key) & 0x8000)
                        {
                            // reserved by cheat, not allowed to bind
                            if (key == VK_DELETE ||
                                key == VK_HOME ||
                                key == VK_END)
                            {
                                NOTIFY_ERROR_QUICK(LANG("Menu_KeyReserved"));
                                break;
                            }

                            MenuToggleKey = key;
                            ImGui::CloseCurrentPopup();
                            break;
                        }
                    }

                    ImGui::EndPopup();
                }
            }
            ImGui::EndChild();

            ImGui::EndGroup();
        }
        break;
        case 2:
        {
            ImGui::SetCursorPosY(20 * dpiScale);
            ImGui::SetCursorPosX(25 * dpiScale);
            ImGui::BeginGroup();

            ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
            ImGui::TextColored(accentPurple, LANG("Menu_About"));
            ImGui::PopFont();
            ImGui::Separator();
            ImGui::Dummy(ImVec2(0, 15));

            ImGui::BeginChild("OpenSource", ImVec2(0, 150 * dpiScale), false);
            {
                ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[7]);
                ImGui::TextColored(accentPurple, LANG("Menu_OpenSource"));
                ImGui::PopFont();
                ImGui::Separator();
                ImGui::Dummy(ImVec2(0, 9));

                ImGui::TextColored(ImVec4(0.6f, 0.8f, 1.0f, 1.0f), LANG("Menu_GitHubRepo"));
                ImGui::SameLine(200 * dpiScale);

                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.3f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.3f, 0.4f, 1.0f));

                if (ImGui::Button(LANG("Menu_OpenGitHub"), ImVec2(120 * dpiScale, 25 * dpiScale)))
                {
                    ShellExecuteA(0, "open", "https://github.com/ViniLog789/PhasmoCheatV-Recode", 0, 0, SW_SHOW);
                }

                ImGui::PopStyleColor(2);

                ImGui::Dummy(ImVec2(0, 5));
                ImGui::TextWrapped(LANG("Menu_OpenSourceDesc"));
            }
            ImGui::EndChild();

            ImGui::Dummy(ImVec2(0, 20));

            ImGui::BeginChild("Credits", ImVec2(0, 0), false);
            {
                ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[7]);
                ImGui::TextColored(accentPurple, LANG("Menu_Credits"));
                ImGui::PopFont();
                ImGui::Separator();
                ImGui::Dummy(ImVec2(0, 10));

                ImGui::Columns(2, "TeamColumns", false);
                ImGui::SetColumnWidth(0, 250 * dpiScale);

                ImGui::BulletText("ViniLog - Lead Coder");
                ImGui::BulletText("Nastya - Designer");
                ImGui::BulletText("Anna - Designer");
                ImGui::BulletText("Artur - Lead Coder");

                ImGui::NextColumn();

                ImGui::BulletText("Kirill - Assistant Lead");
                ImGui::BulletText("Maxim - Coder");
                ImGui::BulletText("Nikita - Coder");
                ImGui::BulletText("Vanya - Coder");
                ImGui::BulletText("Ilya - Tester");
                ImGui::BulletText("James - Translator (RU/EN)");

                ImGui::Columns(1);

                ImGui::Dummy(ImVec2(0, 8));
                ImGui::TextColored(ImVec4(0.6f, 0.8f, 1.0f, 1.0f), LANG("Menu_SpecialThanks"));
                ImGui::TextWrapped("@DashaAngelBars, @TraKKRR_lIo, @MT_FORGET, Evelien, @nypdgov, @nezuukichi, @LingQiao1206, .gashopeless, softvoidds");
            }
            ImGui::EndChild();

            ImGui::EndGroup();
        }
        break;
        case 3:
        {
            ImGui::SetCursorPosY(20 * dpiScale);
            ImGui::SetCursorPosX(25 * dpiScale);

            ImGui::BeginGroup();

            ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
            ImGui::TextColored(accentPurple, LANG("Menu_Hotkeys"));
            ImGui::PopFont();

            ImGui::Separator();
            ImGui::Dummy(ImVec2(0, 10));

            ImGui::BeginChild("HotkeysList", ImVec2(0, 0), false);
            {
                if (BindSystem::Binds.empty())
                {
                    ImGui::SetCursorPos(ImVec2(
                        (ImGui::GetWindowWidth() - ImGui::CalcTextSize("No binds").x) * 0.5f,
                        (ImGui::GetWindowHeight() - ImGui::GetTextLineHeight()) * 0.5f
                    ));
                    ImGui::TextColored(ImVec4(1.f, 0.4f, 0.4f, 1.f), LANG("NoBinds"));
                }
                else
                {
                    for (auto& [uniqueKey, bind] : BindSystem::Binds)
                    {
                        std::string featureName = uniqueKey.substr(0, uniqueKey.find("##"));
                        std::string displayName;

                        std::string headerKey = featureName + "_Header";
                        const char* translated = LANG(headerKey.c_str());
                        if (strcmp(translated, headerKey.c_str()) != 0)
                            displayName = translated;
                        else {
                            const char* btnTranslated = LANG(featureName.c_str());
                            displayName = (strcmp(btnTranslated, featureName.c_str()) != 0) ? btnTranslated : featureName;
                        }

                        bool isWaiting = (BindSystem::WaitingBind.has_value() && BindSystem::WaitingBind.value() == uniqueKey);

                        ImGui::PushID(uniqueKey.c_str());

                        ImGui::Text("%s", displayName.c_str());

                        float regionWidth = ImGui::GetContentRegionAvail().x;
                        ImGui::SameLine(ImGui::GetCursorPosX() + regionWidth - 90);

                        std::string buttonText = isWaiting ? "..." : BindSystem::KeyToString(bind.Key);

                        if (isWaiting)
                        {
                            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.2f, 0.2f, 1.0f));
                            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.3f, 0.3f, 1.0f));
                        }

                        if (ImGui::Button(buttonText.c_str(), ImVec2(80 * dpiScale, 25 * dpiScale)))
                        {
                            if (!BindSystem::WaitingBind.has_value())
                                BindSystem::WaitingBind = uniqueKey;
                            else if (BindSystem::WaitingBind.value() == uniqueKey)
                                BindSystem::WaitingBind.reset();
                        }

                        if (isWaiting)
                            ImGui::PopStyleColor(2);

                        if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
                        {
                            bind.Key = 0;
                            if (isWaiting)
                                BindSystem::WaitingBind.reset();
                        }

                        ImGui::Separator();
                        ImGui::PopID();
                    }
                }
            }
            ImGui::EndChild();

            ImGui::EndGroup();
        }
        break;
        case 4: //! tech part not supported more languages. Only English
            if (IsDebugging)
            {
                ImGui::BeginChild("TestContent", ImVec2(0, 0), false);

                if (ImGui::Button("Dump contracts (map + id)"))
                {
					auto mainManager = SDK::MainManager_staticFields->instance;
					if (!mainManager)
					{
						LOG_ERROR("MainManager instance is null");
						return;
					}

                    auto levelSelection = mainManager->Fields.levelSelection;

                    auto* array = reinterpret_cast<SDK::ContractsArray*>(levelSelection->Fields.contracts);

                    if (!array || array->max_length == 0)
                    {
                        LOG_ERROR("Empty contracts");
                        return;
                    }

                    for (uint32_t i = 0; i < array->max_length; i++)
                    {
                        auto* contract = array->vector[i];
                        if (!contract || !contract->Fields.info)
                            continue;

                        auto* mapName = contract->Fields.info->Fields.mapName;
                        auto uniqueId = contract->Fields.info->Fields.uniqueMapID;

                        std::string mapNameStr = Utils::UnityStrToSysStr(*mapName);

                        LOG_INFO("[", i, "] map=", mapNameStr, " id=", uniqueId);
                    }
                }

                if (ImGui::Button("Test isingame"))
                {
                    bool inGame =
                        SDK::GameController_StaticFields->instance &&
                        SDK::GameController_StaticFields->instance->Fields.allPlayersAreConnected &&
                        Utils::GetGhostAI() &&
                        Utils::GetGhostAI()->Fields.GhostInfo &&
                        Utils::GetGhostAI()->Fields.GhostInfo->Fields.GhostTraits.Name;

					LOG_INFO("Is in game: ", inGame);

                    LOG_INFO("===");

					LOG_INFO("GameController instance: ", SDK::GameController_StaticFields->instance);
					LOG_INFO("allPlayersAreConnected: ", SDK::GameController_StaticFields->instance ? SDK::GameController_StaticFields->instance->Fields.allPlayersAreConnected : false);
					LOG_INFO("GhostAI: ", Utils::GetGhostAI());
					LOG_INFO("GhostInfo: ", Utils::GetGhostAI() ? Utils::GetGhostAI()->Fields.GhostInfo : nullptr);
					LOG_INFO("GhostName_: ", Utils::GetGhostAI() && Utils::GetGhostAI()->Fields.GhostInfo ? Utils::GetGhostAI()->Fields.GhostInfo->Fields.GhostTraits.Name : nullptr);
					LOG_INFO(Utils::GetActiveSceneName());
                }

                if (ImGui::Button("Call test"))
                {
                    ForTestsFlag = true;
                }

                if (ImGui::Button("Crash game"))
                {
                    *(int*)0 = 42;
                }

                ImGui::EndChild();
            }
            break;
        }
    }
    ImGui::EndChild();
    ImGui::EndChild();
    ImGui::End();
}

void Menu::Toggle()
{
    menu.Open = !menu.Open;

    if (menu.Open)
    {
        previousCursorLockMode = SDK::Cursor_Get_LockState(nullptr);
        SDK::Cursor_Set_Visible(true, nullptr);
        SDK::Cursor_Set_LockState(SDK::CursorLockMode::None, nullptr);
    }
    else
    {
        if (previousCursorLockMode != SDK::CursorLockMode::None)
            SDK::Cursor_Set_Visible(false, nullptr);

        SDK::Cursor_Set_LockState(previousCursorLockMode, nullptr);
    }
}