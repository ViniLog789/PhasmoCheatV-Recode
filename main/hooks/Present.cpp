#include "../includes.h"
#include "../features/features_includes.h"
#include "../config/BindSystem.h"
#include "../menu/effects/directx_blur.h"

using namespace PhasmoCheatV;

bool IsFocused()
{
    return GetForegroundWindow() == Renderer::Window;
}

HRESULT __stdcall Hooks::HkPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
{
    if (!menu.Initialized)
    {
        if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D11Device), reinterpret_cast<void**>(&renderer->Device))))
        {
            Renderer::Swapchain = pSwapChain;
            pSwapChain->GetDevice(__uuidof(ID3D11Device), reinterpret_cast<void**>(&Renderer::Device));
            Renderer::Device->GetImmediateContext(&Renderer::Context);

            DXGI_SWAP_CHAIN_DESC description;
            pSwapChain->GetDesc(&description);
            Renderer::Window = description.OutputWindow;

            ImGui::CreateContext();
            ImGui_ImplWin32_Init(Renderer::Window);
            ImGui_ImplDX11_Init(Renderer::Device, Renderer::Context);
            CALL_METHOD_ARGS(Visuals, GhostESP, SetD3D11Device, Renderer::Device); // Photo ESP
            ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
            ImGui::LoadIniSettingsFromDisk((Utils::GetPhasmoCheatVDirectory() + "\\menu.ini").c_str());
            ImGui::GetIO().FontGlobalScale = dpiScale;
            ID3D11Texture2D* pBackBuffer;
            pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<LPVOID*>(&pBackBuffer));
            Renderer::Device->CreateRenderTargetView(pBackBuffer, nullptr, &Renderer::TargetView);
            pBackBuffer->Release();

            hooking->OriginalWndproc = reinterpret_cast<WNDPROC>(SetWindowLongPtr(Renderer::Window, GWLP_WNDPROC,
                reinterpret_cast<LONG_PTR>(HkWndProc)));
            Menu::Initialize();
        }
        else
            return hooking->OriginalPresent(pSwapChain, SyncInterval, Flags);
    }
    ID3D11RenderTargetView* original_target;
    ID3D11DepthStencilView* original_depth;
    Renderer::Context->OMGetRenderTargets(1, &original_target, &original_depth);
    Renderer::Context->OMSetRenderTargets(1, &Renderer::TargetView, nullptr);
    

    if (menu.Open && Globals::IsMenuBlur && Globals::CheatWork) dx_blur_update();

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    if (menu.Open && Globals::CheatWork)
    {
        Menu::Render();
    }

    if (IsFocused() && Globals::CheatWork)
        BindSystem::ProcessBinds();

    if (GET_FEATURE_HANDLER() && Globals::CheatWork)
    {
        GET_FEATURE_HANDLER()->RenderAll();
    }

    if (Globals::CheatWork)
        Notifications::RenderNotifications();

    ImGui::EndFrame();
    ImGui::Render();

    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    Renderer::Context->OMSetRenderTargets(1, &original_target, original_depth);
    if (original_target) original_target->Release();
    if (original_depth) original_depth->Release();

    return hooking->OriginalPresent(pSwapChain, SyncInterval, Flags);
}