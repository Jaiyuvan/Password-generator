#include <string>
#include <iostream>
#include<tuple>
#include "generator.h"
#include "imgui/imgui.h"
#include<glad/glad.h>
#include<glfw3.h>
#include<imgui_impl_glfw.h>
#include<imgui_impl_opengl3.h>



int main()
{

    
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GL_TRUE);
    //glfwWindowHint(GLFW_DECORATED, GL_FALSE);


    GLFWwindow* window = glfwCreateWindow(600, 400, "Password Generator", NULL, NULL);

    if (window == NULL) {
        std::cout << "Error while window creation";
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    glfwMakeContextCurrent(window);

    gladLoadGL();

    glViewport(0, 0, 600, 400);

    //Imgui code
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    ImFontConfig config;
    config.PixelSnapH = true;  
    config.OversampleH = 3;    
    config.OversampleV = 1;

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
    double entropyAns = 0;
    // 4. Main Event Loop
    
    while (!glfwWindowShouldClose(window))
    {
       
        
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
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

        ImGui::SliderInt("Length", &pass_length,4,48);
        ImGui::Checkbox("Include Symbols", &include_symbols);
        ImGui::Checkbox("Include Numbers", &include_numbers);
        
        if (ImGui::Button("Generate Password", ImVec2(-1, 35)))
        {
            std::tuple returnValue = passwordGenerator(include_symbols, include_numbers, pass_length);
            auto [passw, entropy] = returnValue;
            strcpy_s(result_buffer, passw.c_str());
            ImGui::Text("Entropy: %f", entropy);
            entropyAns = entropy;
        }

        ImGui::Separator();
        ImGui::Text("Generated Password:");
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10.0f, 8.0f));
        ImGui::SetNextItemWidth(-1.0f);
        ImGui::InputText("##output", result_buffer, sizeof(result_buffer), ImGuiInputTextFlags_ReadOnly);
        ImGui::PopStyleVar();
        ImGui::Text("Entropy: %f", entropyAns);
        

        if (lato) ImGui::PopFont();

        ImGui::End();

        ImGui::Render();

        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.2039f, 0.2863f, 0.3686f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
    


    return 0;
}
