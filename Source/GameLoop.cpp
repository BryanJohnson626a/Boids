// Prototype code

#include <vector>
#include <iostream>

#include "Boids.h"
#include "GameLoop.h"
#include "Engine/Dice.h"
#include "Engine/Graphics.h"

#ifndef NDEBUG
const int DEFAULT_NUM_BOIDS = 1000;
#else
const int DEFAULT_NUM_BOIDS = 20000;
#endif

const float RATIO = 1;

BoidController * BoidsType1 = nullptr;
BoidController * BoidsType2 = nullptr;

void GameInit(std::vector<std::string> cmd_args)
{
    int num_boid1, num_boid2;

    if (cmd_args.size() == 2)
    {
        int total = std::stoi(cmd_args[1]);
        num_boid1 = static_cast<int>(std::floor((float) total * RATIO));
        num_boid2 = static_cast<int>(std::ceil((float) total * (1 - RATIO)));
    }
    else if (cmd_args.size() == 3)
    {
        num_boid1 = std::stoi(cmd_args[1]);
        num_boid2 = std::stoi(cmd_args[2]);
    }
    else
    {
        num_boid1 = static_cast<int>(std::floor(DEFAULT_NUM_BOIDS * RATIO));
        num_boid2 = static_cast<int>(std::ceil(DEFAULT_NUM_BOIDS * (1 - RATIO)));
    }


    // Make first boid type.
    BoidsType1 = new BoidController("../Resources/Models/lpfish.obj");
    BoidsType1->AvoidFactor = 0.25f;
    BoidsType1->AlignFactor = 1;
    BoidsType1->CohesionFactor = 1;
    BoidsType1->AreaFactor = 1.f/ 100;
    BoidsType1->AreaSize = 30;
    BoidsType1->FearFactor = 1;
    BoidsType1->BoidScale = PE::Vec3{.15f};
    BoidsType1->SetMaterial(PE::turquoise);
    BoidsType1->AddBoids(num_boid1);

    PE::Graphics::GetInstance()->AddModel(BoidsType1);

    // Make second boid type.
    BoidsType2 = new BoidController("../Resources/Models/lpfish.obj");
    BoidsType2->AvoidFactor = 2;
    BoidsType2->AlignFactor = 1;
    BoidsType2->CohesionFactor = 2;
    BoidsType2->AreaFactor = 5;
    BoidsType2->BoidScale = PE::Vec3{.75f};
    BoidsType2->SetMaterial(PE::ruby);
    BoidsType2->AddBoids(num_boid2);

    PE::Graphics::GetInstance()->AddModel(BoidsType2);
}

bool GameLoop(float dt)
{
    static int frame_counter = 0;
    static float counter = 0;
    ++frame_counter;
    counter += dt;
    if (counter >= 1)
    {
        std::cout <<frame_counter << std::endl;
        frame_counter = 0;
        counter -= 1;
    }

    BoidsType1->Update(dt);
    BoidsType2->Update(dt);

    SDL_Event e;
    while (SDL_PollEvent(&e))
    {
        switch (e.type)
        {
            case SDL_KEYDOWN:
                switch (e.key.keysym.sym)
                {
                    case SDLK_1:
                        BoidsType1->RemoveBoids(100);
                        break;
                    case SDLK_2:
                        BoidsType1->AddBoids(100);
                        break;
                    case SDLK_3:
                        BoidsType2->RemoveBoids(10);
                        break;
                    case SDLK_4:
                        BoidsType2->AddBoids(10);
                        break;
                    case SDLK_F1:
                        PE::Graphics::GetInstance()->debug_mode = 1;
                        break;
                    case SDLK_F2:
                        PE::Graphics::GetInstance()->debug_mode = 2;
                        break;
                    case SDLK_F3:
                        PE::Graphics::GetInstance()->debug_mode = 3;
                        break;
                    case SDLK_F4:
                        PE::Graphics::GetInstance()->debug_mode = 4;
                        break;
                    case SDLK_F5:
                        PE::Graphics::GetInstance()->debug_mode = 5;
                        break;
                    case SDLK_F6:
                        PE::Graphics::GetInstance()->debug_mode = 6;
                        break;
                    case SDLK_F10:
                        PE::Graphics::GetInstance()->debug_mode = 0;
                        break;
                    case SDLK_F11:
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
    delete BoidsType1;
    delete BoidsType2;
}

