#include <glad/glad.h>
#include <glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <cstring>
#include <iostream>
#include <string>
#include <tuple>

#include "generator.h"

int main() {
    if (!glfwInit()) {
        std::cout << "Failed to initialize GLFW\n";
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(800, 500, "Password Generator", NULL, NULL);

    if (window == NULL) {
        std::cout << "Error while window creation\n";
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    gladLoadGL();
    glViewport(0, 0, 600, 400);

    // ImGui Initialization
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = nullptr;

    ImFontConfig config;
    config.PixelSnapH = true;
    config.OversampleH = 3;
    config.OversampleV = 1;

    ImFont* lato = io.Fonts->AddFontFromFileTTF("Lato-Black.ttf", 18.0f, &config);
    if (lato == nullptr) {
        io.Fonts->AddFontDefault();
    }

    // --- UI State Variables ---
    char result_buffer[128] = "";
    char result_phrase[512] = "";
    int pass_length = 16;
    int word_length = 5;
    bool include_symbols = true;
    bool include_numbers = true;
    double entropyAns = 0.0;

    // --- Main Loop ---
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f), ImGuiCond_Always);
        ImGui::SetNextWindowSize(io.DisplaySize, ImGuiCond_Always);

        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar
            | ImGuiWindowFlags_NoResize
            | ImGuiWindowFlags_NoMove
            | ImGuiWindowFlags_NoCollapse;

        ImGui::Begin("Generator Controls", nullptr, window_flags);

        if (lato) ImGui::PushFont(lato);

        // Password Generator Section
        ImGui::Text("Password Generator");
        ImGui::SliderInt("Length", &pass_length, 4, 56);
        ImGui::Checkbox("Include Symbols", &include_symbols);
        ImGui::Checkbox("Include Numbers", &include_numbers);

        if (ImGui::Button("Generate Password", ImVec2(-1.0f, 35.0f))) {
            auto [passw, entropy] = passwordGenerator(include_symbols, include_numbers, pass_length);
            snprintf(result_buffer, sizeof(result_buffer), "%s", passw.c_str());
            entropyAns = entropy;
        }

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10.0f, 8.0f));
        ImGui::SetNextItemWidth(-1.0f);
        ImGui::InputText("##pass_output", result_buffer, sizeof(result_buffer), ImGuiInputTextFlags_ReadOnly);
        if (ImGui::Button("Copy", ImVec2(-1.0f, 35.0f))) {
            glfwSetClipboardString(window, result_buffer);
        }
        ImGui::PopStyleVar();

        ImGui::Text("Entropy: %.2f bits", entropyAns);

        ImGui::Separator();

        
        ImGui::Text("Passphrase Generator");
        ImGui::SliderInt("Word Length", &word_length, 3, 12);

        if (ImGui::Button("Generate Passphrase", ImVec2(-1.0f, 35.0f))) {
            std::string phrase = passByPhrase(word_length);
            snprintf(result_phrase, sizeof(result_phrase), "%s", phrase.c_str());
        }

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10.0f, 8.0f));
        ImGui::SetNextItemWidth(-1.0f);
        ImGui::InputText("##phrase_output", result_phrase, sizeof(result_phrase), ImGuiInputTextFlags_ReadOnly);
        if (ImGui::Button("Copy Pass by Phrase", ImVec2(-1.0f, 35.0f))) {
            glfwSetClipboardString(window, result_phrase);
        }
        ImGui::PopStyleVar();

        if (lato) ImGui::PopFont();

        ImGui::End();

        // --- Rendering ---
        ImGui::Render();

        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);

        glClearColor(0.2039f, 0.2863f, 0.3686f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}