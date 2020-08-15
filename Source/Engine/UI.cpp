// Created by Bryan Johnson on 8/14/2020.

#include <SDL2/SDL_video.h>
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl.h"
#include "UI.h"
#include "Color.h"
#include "../GameUI.h"


namespace PE
{
    void SetupImgui(SDL_Window *window, SDL_GLContext gl_context)
    {
        // Setup Dear ImGui binding
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        
        ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
        ImGui_ImplOpenGL3_Init();
        
        // Setup style
        ImGui::StyleColorsDark();
        
    }
    
    void UpdateImgui(SDL_Window *window)
    {
        // Start the ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame(window);
        ImGui::NewFrame();
        
        if (GameUI::GetGameUI() != nullptr)
            GameUI::GetGameUI()->UpdateGameUI();
        
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        
    }
    
    void CleanupImgui()
    {
        // Cleanup
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();
    }
}