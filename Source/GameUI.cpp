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
const float FearScale = .000002f;
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

void DisplayBoidUI(BoidController * bc)
{
    // Imgui requires a unique label for every button/slider/etc.
    // If two bcs share a name they will control each other's buttons.
    std::string uid = bc->Name;
    
    std::string label = bc->Name + " settings";
    ImGui::Text("%s", label.c_str());
    
    int num_boids = bc->GetNumBoids();
    label = "# of Boids##" + uid;
    ImGui::SliderInt(label.c_str(), &num_boids, 0, 100000);
    int diff = num_boids - (int) bc->GetNumBoids();
    if (diff > 0)
        bc->AddBoids(diff);
    else if (diff < 0)
        bc->RemoveBoids(-diff);
    
    ImGui::Text("Boid Behavior");
    
    float scaled_avoid_factor = bc->AvoidFactor * AvoidScale;
    label = "Avoidance##" + uid;
    ImGui::SliderFloat(label.c_str(), &scaled_avoid_factor, 0, 10);
    bc->AvoidFactor = scaled_avoid_factor / AvoidScale;
    
    float scaled_align_factor = bc->AlignFactor * AlignScale;
    label = "Alignment##" + uid;
    ImGui::SliderFloat(label.c_str(), &scaled_align_factor, 0, 10);
    bc->AlignFactor = scaled_align_factor / AlignScale;
    
    float scaled_cohesion_factor = bc->CohesionFactor * CohesionScale;
    label = "Cohesion##" + uid;
    ImGui::SliderFloat(label.c_str(), &scaled_cohesion_factor, 0, 10);
    bc->CohesionFactor = scaled_cohesion_factor / CohesionScale;
    
    float scaled_fear_factor = bc->FearFactor * FearScale;
    label = "Fear##" + uid;
    ImGui::SliderFloat(label.c_str(), &scaled_fear_factor, -10, 10);
    bc->FearFactor = scaled_fear_factor / FearScale;
    
    float scaled_area_factor = bc->AreaFactor * AreaScale;
    label = "Containment##" + uid;
    ImGui::SliderFloat(label.c_str(), &scaled_area_factor, 0, 10);
    bc->AreaFactor = scaled_area_factor / AreaScale;
    
    ImGui::Text("Boid Properties");
    
    float scaled_speed = bc->Speed * SpeedScale;
    label = "Speed##" + uid;
    ImGui::SliderFloat(label.c_str(), &scaled_speed, 0.01, 10);
    bc->Speed = scaled_speed / SpeedScale;
    
    float scaled_turn_force = bc->TurnForce * TurnForceScale;
    label = "Maneuver##" + uid;
    ImGui::SliderFloat(label.c_str(), &scaled_turn_force, 0.01, 10);
    bc->TurnForce = scaled_turn_force / TurnForceScale;
    
    ImGui::Text("Boid Colors");
    label = "White##" + uid;
    if (ImGui::Button(label.c_str()))
        SetColorsWhite(bc);
    ImGui::SameLine();
    label = "Plastic##" + uid;
    if (ImGui::Button(label.c_str()))
        SetColorsPlastic(bc);
    ImGui::SameLine();
    label = "Rubber##" + uid;
    if (ImGui::Button(label.c_str()))
        SetColorsRubber(bc);
    ImGui::SameLine();
    label = "Metal##" + uid;
    if (ImGui::Button(label.c_str()))
        SetColorsMetal(bc);
    ImGui::SameLine();
    label = "Gemstone##" + uid;
    if (ImGui::Button(label.c_str()))
        SetColorsGemstone(bc);
    
    ImGui::Text("Container");
    
    label = "Continuous##" + uid;
    ImGui::Checkbox(label.c_str(), &bc->ContinuousContainer);
    ImGui::SameLine();
    label = "Hard Barrier##" + uid;
    ImGui::Checkbox(label.c_str(), &bc->HardContainer);
    
    float scaled_area_size = bc->GetAreaSize() * AreaSizeScale;
    label = "Size##" + uid;
    ImGui::SliderFloat(label.c_str(), &scaled_area_size, 0.01, 10);
    bc->SetAreaSize(scaled_area_size / AreaSizeScale);
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
    
    for (auto * bc : BoidControllers)
    {
        ImGui::Separator();
        DisplayBoidUI(bc);
    }
    
}

GameUI * GameUI::GetGameUI()
{
    return instance;
}

void GameUI::SetInstance(GameUI * game_ui)
{
    instance = game_ui;
}
