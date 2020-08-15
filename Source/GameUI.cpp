// Created by bushk on 8/15/2020.

#include <imgui.h>
#include "GameUI.h"
#include "Boids.h"

GameUI * GameUI::instance = nullptr;

const float AreaSizeScale = 0.1f;
const float AvoidScale = 8.f;
const float AlignScale = 2.f;
const float CohesionScale = 2.f;
const float AreaScale = 4000.f;
const float SpeedScale = 1.f;
const float TurnForceScale = 1.f;


void SetColorsWhite(BoidController * bc)
{
    bc->ClearBoidMaterials();
    bc->AddBoidMaterial(PE::white_plastic);
}

void SetColorsGemstone(BoidController * bc)
{
    bc->ClearBoidMaterials();
    bc->AddBoidMaterial(PE::turquoise);
    bc->AddBoidMaterial(PE::emerald);
    bc->AddBoidMaterial(PE::jade);
    bc->AddBoidMaterial(PE::obsidian);
    bc->AddBoidMaterial(PE::pearl);
    bc->AddBoidMaterial(PE::ruby);
}

void SetColorsMetal(BoidController * bc)
{
    bc->ClearBoidMaterials();
    bc->AddBoidMaterial(PE::brass);
    bc->AddBoidMaterial(PE::bronze);
    bc->AddBoidMaterial(PE::chrome);
    bc->AddBoidMaterial(PE::copper);
    bc->AddBoidMaterial(PE::gold);
    bc->AddBoidMaterial(PE::silver);
}

void SetColorsPlastic(BoidController * bc)
{
    bc->ClearBoidMaterials();
    bc->AddBoidMaterial(PE::black_plastic);
    bc->AddBoidMaterial(PE::cyan_plastic);
    bc->AddBoidMaterial(PE::green_plastic);
    bc->AddBoidMaterial(PE::red_plastic);
    bc->AddBoidMaterial(PE::white_plastic);
    bc->AddBoidMaterial(PE::yellow_plastic);
}

void SetColorsRubber(BoidController * bc)
{
    bc->ClearBoidMaterials();
    bc->AddBoidMaterial(PE::black_rubber);
    bc->AddBoidMaterial(PE::cyan_rubber);
    bc->AddBoidMaterial(PE::green_rubber);
    bc->AddBoidMaterial(PE::red_rubber);
    bc->AddBoidMaterial(PE::white_rubber);
    bc->AddBoidMaterial(PE::yellow_rubber);
}

void GameUI::UpdateGameUI()
{
    static std::vector<float> FPS(100, 60);
    static uint FPS_index = 0;
    FPS[FPS_index] = ImGui::GetIO().Framerate;
    FPS_index = (FPS_index + 1) % 100;
    float avg_fps = 0;
    for (float fps : FPS)
        avg_fps += fps;
    avg_fps /= 100;
    
    ImGui::Begin("Control Panel");
    
    ImGui::Text("Press Esc to quit.");
    
    ImGui::Spacing();
    
    ImGui::Text("Average performance: %.1f ms/frame (%.1f FPS)", 1000.f / avg_fps, avg_fps);
    
    ImGui::Separator();
    ImGui::Text("Container Settings");
    
    ImGui::Checkbox("Continuous", &bc1->ContinuousContainer);
    ImGui::SameLine();
    ImGui::Checkbox("Hard Barrier", &bc1->HardContainer);
    
    float scaled_area_size = bc1->GetAreaSize() * AreaSizeScale;
    ImGui::SliderFloat("Size", &scaled_area_size, 0.01, 10);
    bc1->SetAreaSize(scaled_area_size / AreaSizeScale);
    
    ImGui::Separator();
    ImGui::Text("Boid Type 1");
    
    int num_boids = bc1->GetNumBoids();
    ImGui::SliderInt("# of Boids", &num_boids, 0, 100000);
    int diff = num_boids - (int) bc1->GetNumBoids();
    if (diff > 0)
        bc1->AddBoids(diff);
    else if (diff < 0)
        bc1->RemoveBoids(-diff);
    
    ImGui::Separator();
    ImGui::Text("Boid Behavior");
    
    float scaled_avoid_factor = bc1->AvoidFactor * AvoidScale;
    ImGui::SliderFloat("Avoidance", &scaled_avoid_factor, 0, 10);
    bc1->AvoidFactor = scaled_avoid_factor / AvoidScale;
    
    float scaled_align_factor = bc1->AlignFactor * AlignScale;
    ImGui::SliderFloat("Alignment", &scaled_align_factor, 0, 10);
    bc1->AlignFactor = scaled_align_factor / AlignScale;
    
    float scaled_cohesion_factor = bc1->CohesionFactor * CohesionScale;
    ImGui::SliderFloat("Cohesion", &scaled_cohesion_factor, 0, 10);
    bc1->CohesionFactor = scaled_cohesion_factor / CohesionScale;
    
    float scaled_area_factor = bc1->AreaFactor * AreaScale;
    ImGui::SliderFloat("Containment", &scaled_area_factor, 0, 10);
    bc1->AreaFactor = scaled_area_factor / AreaScale;
    
    ImGui::Separator();
    
    ImGui::Text("Boid Properties");
    
    float scaled_speed = bc1->Speed * SpeedScale;
    ImGui::SliderFloat("Speed", &scaled_speed, 0.01, 10);
    bc1->Speed = scaled_speed / SpeedScale;
    
    float scaled_turn_force = bc1->TurnForce * TurnForceScale;
    ImGui::SliderFloat("Maneuver", &scaled_turn_force, 0.01, 10);
    bc1->TurnForce = scaled_turn_force / TurnForceScale;
    
    ImGui::Text("Boid Colors");
    if (ImGui::Button("White"))
        SetColorsWhite(bc1);
    ImGui::SameLine();
    if (ImGui::Button("Plastic"))
        SetColorsPlastic(bc1);
    ImGui::SameLine();
    if (ImGui::Button("Rubber"))
        SetColorsRubber(bc1);
    ImGui::SameLine();
    if (ImGui::Button("Metal"))
        SetColorsMetal(bc1);
    ImGui::SameLine();
    if (ImGui::Button("Gemstone"))
        SetColorsGemstone(bc1);
    
}

GameUI * GameUI::GetGameUI()
{
    return instance;
}

void GameUI::SetInstance(GameUI * game_ui)
{
    instance = game_ui;
}
