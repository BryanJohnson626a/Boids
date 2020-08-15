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

void GameUI::UpdateGameUI()
{
    ImGui::Begin("Control Panel");
    
    ImGui::Text("Press Esc to quit.");
    
    ImGui::Spacing();
    
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate,
                ImGui::GetIO().Framerate);
    
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
    int diff = num_boids - (int)bc1->GetNumBoids();
    if (diff > 0)
        bc1->AddBoids(diff);
    else if (diff < 0)
        bc1->RemoveBoids(-diff);
    
    ImGui::Separator();
    ImGui::Text("Boid Behavior");
    
    float scaled_avoid_factor = bc1->AvoidFactor * AvoidScale;
    ImGui::SliderFloat("Avoidance Factor", &scaled_avoid_factor, 0, 10);
    bc1->AvoidFactor = scaled_avoid_factor / AvoidScale;
    
    float scaled_align_factor = bc1->AlignFactor * AlignScale;
    ImGui::SliderFloat("Alignment Factor", &scaled_align_factor, 0, 10);
    bc1->AlignFactor = scaled_align_factor / AlignScale;
    
    float scaled_cohesion_factor = bc1->CohesionFactor * CohesionScale;
    ImGui::SliderFloat("Cohesion Factor", &scaled_cohesion_factor, 0, 10);
    bc1->CohesionFactor = scaled_cohesion_factor / CohesionScale;
    
    float scaled_area_factor = bc1->AreaFactor * AreaScale;
    ImGui::SliderFloat("Containment Factor", &scaled_area_factor, 0, 10);
    bc1->AreaFactor = scaled_area_factor / AreaScale;
    
    ImGui::Separator();
    
    ImGui::Text("Boid Properties");
    
    float scaled_speed = bc1->Speed * SpeedScale;
    ImGui::SliderFloat("Speed", &scaled_speed, 0.01, 10);
    bc1->Speed = scaled_speed / SpeedScale;
    
    float scaled_turn_force = bc1->TurnForce * TurnForceScale;
    ImGui::SliderFloat("Maneuverability", &scaled_turn_force, 0.01, 10);
    bc1->TurnForce = scaled_turn_force / TurnForceScale;
    
}

GameUI * GameUI::GetGameUI()
{
    return instance;
}

void GameUI::SetInstance(GameUI * game_ui)
{
    instance = game_ui;
}
