// Prototype code

#include <vector>
#include <iostream>

#include "Boids.h"
#include "GameLoop.h"
#include "Engine/Dice.h"
#include "Engine/Graphics.h"
#include "Engine/imgui_impl_sdl.h"
#include "GameUI.h"

#ifndef NDEBUG
const int DEFAULT_NUM_BOIDS = 1000;
const float BOUNDS = 10;
#else
const int DEFAULT_NUM_BOIDS = 50000;
const int DEFAULT_NUM_BOIDS2 = 10;
const float BOUNDS = 60;
#endif

const float RATIO = 1;

std::vector<BoidController *> BoidControllers;
PE::Model * bounding_sphere = nullptr;

GameUI * game_ui = nullptr;

void GameInit(std::vector<std::string> cmd_args)
{
    // Set up Game UI
    game_ui = new GameUI();
    GameUI::SetInstance(game_ui);
    
    // Center sphere.
    //bounding_sphere = new PE::Model("../Resources/Models/sphere.ply");
    //bounding_sphere->SetMaterial(PE::silver);
    //bounding_sphere->ResetRotation();
    //PE::Graphics::GetInstance()->AddModel(bounding_sphere);
    
    // Make first boid type.
    BoidController * BoidsType1 = new BoidController("../Resources/Models/lpfish.obj");
    BoidsType1->Name = "Boid Type 1";
    BoidsType1->AvoidFactor = 0.25f;
    BoidsType1->AlignFactor = 1;
    BoidsType1->CohesionFactor = 1;
    BoidsType1->AreaFactor = 1.f / 2000.f;
    BoidsType1->SetAreaSize(BOUNDS);
    BoidsType1->FearFactor = 1000000;
    BoidsType1->BoidScale = PE::Vec3{.15f};
    BoidsType1->SetNeighborDistance(2);
    BoidsType1->AddBoids(DEFAULT_NUM_BOIDS);
    BoidsType1->AddBoidMaterial(PE::cyan_plastic);
    PE::Graphics::GetInstance()->AddModel(BoidsType1);
    game_ui->BoidControllers.emplace_back(BoidsType1);
    
    // Make second boid type.
    BoidController * BoidsType2 = new BoidController("../Resources/Models/lpfish.obj");
    BoidsType2->Name = "Boid Type 2";
    BoidsType2->AvoidFactor = 0.25f;
    BoidsType2->AlignFactor = 1;
    BoidsType2->CohesionFactor = 1;
    BoidsType2->AreaFactor = 1.f / 2000.f;
    BoidsType2->SetAreaSize(BOUNDS);
    BoidsType2->FearFactor = -1000000;
    BoidsType2->BoidScale = PE::Vec3{1.f};
    BoidsType2->SetNeighborDistance(2);
    BoidsType2->AddBoids(DEFAULT_NUM_BOIDS2);
    BoidsType2->AddBoidMaterial(PE::red_plastic);
    PE::Graphics::GetInstance()->AddModel(BoidsType2);
    game_ui->BoidControllers.emplace_back(BoidsType2);
    
    BoidsType1->AddFearedBoids(BoidsType2);
    BoidsType2->AddFearedBoids(BoidsType1);
}

bool GameLoop(float dt)
{
    static float mouse_speed = 0.001f;
    static float cam_speed = 10;
    
    for (auto * bc : game_ui->BoidControllers)
        bc->Update(dt);
    
    auto keystate = SDL_GetKeyboardState(nullptr);
    if (keystate[SDL_SCANCODE_W] || keystate[SDL_SCANCODE_UP])
        PE::Graphics::GetInstance()->MoveCameraRelative(
                PE::Vec3{dt * cam_speed, 0, 0});
    if (keystate[SDL_SCANCODE_S] || keystate[SDL_SCANCODE_DOWN])
        PE::Graphics::GetInstance()->MoveCameraRelative(
                PE::Vec3{-dt * cam_speed, 0, 0});
    if (keystate[SDL_SCANCODE_A] || keystate[SDL_SCANCODE_LEFT])
        PE::Graphics::GetInstance()->MoveCameraRelative(
                PE::Vec3{0, -dt * cam_speed, 0});
    if (keystate[SDL_SCANCODE_D] || keystate[SDL_SCANCODE_RIGHT])
        PE::Graphics::GetInstance()->MoveCameraRelative(
                PE::Vec3{0, dt * cam_speed, 0});
    if (keystate[SDL_SCANCODE_LCTRL] || keystate[SDL_SCANCODE_Q])
        PE::Graphics::GetInstance()->MoveCameraRelative(
                PE::Vec3{0, 0, dt * cam_speed});
    if (keystate[SDL_SCANCODE_LSHIFT] || keystate[SDL_SCANCODE_E])
        PE::Graphics::GetInstance()->MoveCameraRelative(
                PE::Vec3{0, 0, -dt * cam_speed});
    
    SDL_Event e;
    while (SDL_PollEvent(&e))
    {
        ImGui_ImplSDL2_ProcessEvent(&e);
        
        ImGuiIO & io = ImGui::GetIO();
        
        if (!io.WantCaptureMouse)
            switch (e.type)
            {
                case SDL_MOUSEMOTION:
                {
                    // First person camera movement. Only rotate with the mouse if relative mouse mode is active.
                    // Also skip the first frame of movement since it's always some high value.
                    static bool first_frame = true;
                    if (SDL_GetRelativeMouseMode() && !first_frame)
                    {
                        auto facing = PE::Graphics::GetInstance()->cam_facing;
                        auto relative_horizontal = glm::normalize(glm::cross(facing, PE::Up));
                        PE::Graphics::GetInstance()->RotateCamera(-e.motion.xrel * mouse_speed, PE::Up);
                        PE::Graphics::GetInstance()->RotateCamera(-e.motion.yrel * mouse_speed,
                                                                  relative_horizontal);
                    }
                    else first_frame = false;
                }
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    SDL_SetRelativeMouseMode(SDL_TRUE);
                    break;
                case SDL_MOUSEBUTTONUP:
                    SDL_SetRelativeMouseMode(SDL_FALSE);
                    break;
                
                case SDL_KEYDOWN:
                    switch (e.key.keysym.sym)
                    {
                        case SDLK_ESCAPE:
                            return false;
                        case SDLK_F1:
                            PE::Graphics::GetInstance()->debug_mode = 0;
                            break;
                        case SDLK_F2:
                            PE::Graphics::GetInstance()->debug_mode = 1;
                            break;
                        case SDLK_F3:
                            PE::Graphics::GetInstance()->debug_mode = 2;
                            break;
                        case SDLK_F4:
                            PE::Graphics::GetInstance()->debug_mode = 3;
                            break;
                        case SDLK_F5:
                            PE::Graphics::GetInstance()->debug_mode = 4;
                            break;
                        case SDLK_F6:
                            PE::Graphics::GetInstance()->debug_mode = 5;
                            break;
                        case SDLK_F7:
                            PE::Graphics::GetInstance()->debug_mode = 6;
                            break;
                        case SDLK_F12:
                            PE::Graphics::GetInstance()->CompileShaders();
                            break;
                        default:
                            break;
                    }
                    break;
                case SDL_QUIT:
                    return false;
                default:
                    break;
            }
    }
    return true;
}

void GameShutdown()
{
    for (auto * bc : game_ui->BoidControllers)
        delete bc;
}

