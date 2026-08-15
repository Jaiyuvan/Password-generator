#include <windows.h>
#include <d3d11.h>
#include <string>
#include <iostream>

#include "generator.h"
#include "imgui/imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

// DirectX 11 Globals
static ID3D11Device* g_pd3dDevice = nullptr;
static ID3D11DeviceContext* g_pd3dDeviceContext = nullptr;
static IDXGISwapChain* g_pSwapChain = nullptr;
static ID3D11RenderTargetView* g_mainRenderTargetView = nullptr;

// Declarations
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

int main()
{
    // 1. Create Window
    WNDCLASSEXW wc = { sizeof(wc), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, L"ImGui Class", nullptr };
    ::RegisterClassExW(&wc);
    HWND hwnd = ::CreateWindowW(wc.lpszClassName, L"Password Generator", WS_OVERLAPPEDWINDOW, 100, 100, 450, 350, nullptr, nullptr, wc.hInstance, nullptr);

    // 2. Init DirectX
    if (!CreateDeviceD3D(hwnd)) {
        CleanupDeviceD3D();
        ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
        return 1;
    }

    ::ShowWindow(hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hwnd);

    // 3. Init ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    
    ImGui::StyleColorsDark();
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = nullptr;
    ImFont* lato = io.Fonts->AddFontFromFileTTF("Lato-Black.ttf", 18.0f);
    if (lato == nullptr) {
        io.Fonts->AddFontDefault();
    }

    // --- UI State Variables ---
    char result_buffer[128] = "";
    int pass_length = 16;
    bool include_symbols = true;
    bool include_numbers = true;

    // 4. Main Event Loop
    bool done = false;
    while (!done)
    {
        MSG msg;
        while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE)) {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT) done = true;
        }
        if (done) break;

        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
        ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f), ImGuiCond_Always);

        ImGui::SetNextWindowSize(io.DisplaySize, ImGuiCond_Always);
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar
            | ImGuiWindowFlags_NoResize
            | ImGuiWindowFlags_NoMove
            | ImGuiWindowFlags_NoCollapse;

        // --- DRAW INTERFACE ---
        ImGui::Begin("Generator Controls", nullptr, window_flags);

        if (lato) ImGui::PushFont(lato);

        ImGui::SliderInt("Length", &pass_length,4,64);
        ImGui::Checkbox("Include Symbols", &include_symbols);
        ImGui::Checkbox("Include Numbers", &include_numbers);

        if (ImGui::Button("Generate Password", ImVec2(-1, 35)))
        {
            std::string pass = passwordGenerator(include_symbols, include_numbers, pass_length);
            strcpy_s(result_buffer, pass.c_str());
        }

        ImGui::Separator();
        ImGui::Text("Generated Password:");
        ImGui::InputText("##output", result_buffer, sizeof(result_buffer), ImGuiInputTextFlags_ReadOnly);

        if (lato) ImGui::PopFont();

        ImGui::End();

        // Render
        ImGui::Render();
        const float clear_color[4] = { 0.2039f, 0.2863f, 0.3686f, 1.00f };
        g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);
        g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
        g_pSwapChain->Present(1, 0);
    }

    // Cleanup
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
    CleanupDeviceD3D();
    ::DestroyWindow(hwnd);
    ::UnregisterClassW(wc.lpszClassName, wc.hInstance);

    return 0;
}

// Boilerplate DirectX Functions
bool CreateDeviceD3D(HWND hWnd) {
    DXGI_SWAP_CHAIN_DESC sd = {};
    sd.BufferCount = 2;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    D3D_FEATURE_LEVEL fl;
    const D3D_FEATURE_LEVEL flArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0 };
    if (D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, flArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &fl, &g_pd3dDeviceContext) != S_OK)
        return false;
    CreateRenderTarget();
    return true;
}

void CleanupDeviceD3D() {
    CleanupRenderTarget();
    if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = nullptr; }
    if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = nullptr; }
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = nullptr; }
}

void CreateRenderTarget() {
    ID3D11Texture2D* pBackBuffer;
    g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
    if (pBackBuffer) {
        g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_mainRenderTargetView);
        pBackBuffer->Release();
    }
}

void CleanupRenderTarget() {
    if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = nullptr; }
}

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam)) return true;
    if (msg == WM_DESTROY) { ::PostQuitMessage(0); return 0; }
    return ::DefWindowProc(hWnd, msg, wParam, lParam);
}