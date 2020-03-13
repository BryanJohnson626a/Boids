// Prototype code

#include <vector>
#include <glm/gtc/matrix_transform.hpp>

#include "Boid.h"
#include "Behavior.h"
#include "GameLoop.h"
#include "Engine/Dice.h"
#include "Engine/Graphics.h"

#ifndef NDEBUG
const int DEFAULT_NUM_BOIDS = 100;
#else
const int DEFAULT_NUM_BOIDS = 1000;
#endif

//const float RATIO = 0.75f;
const float RATIO = 1;

std::vector<Boid *> allBoids;
std::vector<Boid *> boids1;
std::vector<Boid *> boids2;

std::vector<PE::Vertex> bverts{
        PE::Vertex{PE::Vec3{-1, 0, 1}, PE::Vec3{}, PE::Vec2{}},
        PE::Vertex{PE::Vec3{0, .5, .5}, PE::Vec3{}, PE::Vec2{}},
        PE::Vertex{PE::Vec3{0, 0, -1}, PE::Vec3{}, PE::Vec2{}},

        PE::Vertex{PE::Vec3{0, 0, -1}, PE::Vec3{}, PE::Vec2{}},
        PE::Vertex{PE::Vec3{0, .5, .5}, PE::Vec3{}, PE::Vec2{}},
        PE::Vertex{PE::Vec3{1, 0, 1}, PE::Vec3{}, PE::Vec2{}},

        PE::Vertex{PE::Vec3{-1, 0, 1}, PE::Vec3{}, PE::Vec2{}},
        PE::Vertex{PE::Vec3{0, -.5, .5}, PE::Vec3{}, PE::Vec2{}},
        PE::Vertex{PE::Vec3{0, .5, .5}, PE::Vec3{}, PE::Vec2{}},

        PE::Vertex{PE::Vec3{0, -.5, .5}, PE::Vec3{}, PE::Vec2{}},
        PE::Vertex{PE::Vec3{1, 0, 1}, PE::Vec3{}, PE::Vec2{}},
        PE::Vertex{PE::Vec3{0, .5, .5}, PE::Vec3{}, PE::Vec2{}},

        PE::Vertex{PE::Vec3{-1, 0, 1}, PE::Vec3{}, PE::Vec2{}},
        PE::Vertex{PE::Vec3{0, 0, -1}, PE::Vec3{}, PE::Vec2{}},
        PE::Vertex{PE::Vec3{0, -.5, .5}, PE::Vec3{}, PE::Vec2{}},

        PE::Vertex{PE::Vec3{1, 0, 1}, PE::Vec3{}, PE::Vec2{}},
        PE::Vertex{PE::Vec3{0, -.5, .5}, PE::Vec3{}, PE::Vec2{}},
        PE::Vertex{PE::Vec3{0, 0, -1}, PE::Vec3{}, PE::Vec2{}},
};

std::vector<uint> bindices{
        0, 1, 2,
        3, 4, 5,
        6, 7, 8,
        9, 10, 11,
        12, 13, 14,
        15, 16, 17
};

PE::TextureList btex;

static PE::Model * BoidModel{nullptr};

void MakeBoidType1()
{
    static DieReal AvoidFac(0.25f, 0.5f);
    static DieReal AvoidDis(1.25f, 1.5f);
    static DieReal AlignFac(1.f, 1.5f);
    static DieReal AlignDis(1.f, 1.5f);
    static DieReal CohesFac(0.5f, 1.5f);
    static DieReal CohesDis(1.5f, 2.5f);
    static DieReal FearFac(1.f, 1.5f);
    static DieReal FearDis(2.2f, 3.3f);

    auto * b = new Boid(*BoidModel);
    allBoids.emplace_back(b);
    boids1.emplace_back(b);

    b->AddBehavior(new BehaviorAvoid(b, AvoidFac.Roll(), AvoidDis.Roll(), &boids1));
    b->AddBehavior(new BehaviorAlign(b, AlignFac.Roll(), AlignDis.Roll(), &boids1));
    b->AddBehavior(new BehaviorCohesion(b, CohesFac.Roll(), CohesDis.Roll(), &boids1));
    b->AddBehavior(new BehaviorAvoid(b, FearFac.Roll(), FearDis.Roll(), &boids2));
    b->AddBehavior(new BehaviorStayInArea(b, 1, 5, BOUNDS * -0.75f, BOUNDS * 0.75f));

    b->Scale(0.25f);
    b->SetMaterial(PE::white_plastic);
    PE::Graphics::GetInstance()->PE::Graphics::AddModel(b);
}

void MakeBoidType2()
{
    static DieReal AvoidFac(0.25f, 0.5f);
    static DieReal AvoidDis(1.5f, 2.5f);
    static DieReal AlignFac(2.5f, 3.5f);
    static DieReal AlignDis(1.5f, 2.5f);
    static DieReal CohesFac(0.5f, 1.5f);
    static DieReal CohesDis(2.5f, 3.5f);

    auto * b = new Boid(*BoidModel);
    allBoids.emplace_back(b);
    boids2.emplace_back(b);

    b->AddBehavior(new BehaviorAvoid(b, AvoidFac.Roll(), AvoidDis.Roll(), &boids2));
    b->AddBehavior(new BehaviorAlign(b, AlignFac.Roll(), AlignDis.Roll(), &boids1));
    b->AddBehavior(new BehaviorCohesion(b, CohesFac.Roll(), CohesDis.Roll(), &boids1));
    b->AddBehavior(new BehaviorStayInArea(b, 1, 5, BOUNDS * -0.75f, BOUNDS * 0.75f));

    b->Scale(0.5f);
    b->SetMaterial(PE::green_plastic);
    b->SetSpeed(0.75f);
    PE::Graphics::GetInstance()->PE::Graphics::AddModel(b);
}

void GameInit(std::vector<std::string> cmd_args)
{
    for (int i = 0; i < bverts.size(); i += 3)
    {
        PE::Vec3 normal = glm::cross(bverts[i + 1].Position - bverts[i].Position,
                                     bverts[i + 2].Position - bverts[i].Position);
        bverts[i].Normal = normal;
        bverts[i + 1].Normal = normal;
        bverts[i + 2].Normal = normal;
    }

    PE::Mesh BoidMesh = PE::Mesh(bverts, bindices, btex);
    BoidModel = new PE::Model(BoidMesh);

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

    allBoids.reserve(num_boid1 + num_boid2);

    boids1.reserve(num_boid1);
    for (int i = 0; i < num_boid1; ++i)
        MakeBoidType1();

    boids1.reserve(num_boid2);
    for (int i = 0; i < num_boid2; ++i)
        MakeBoidType2();
}

bool GameLoop(float dt)
{
    for (auto b : allBoids)
        b->Update(dt);

    for (auto b : allBoids)
        b->UpdateMove(dt);

    SDL_Event e;
    while (SDL_PollEvent(&e))
    {
        switch (e.type)
        {
            case SDL_KEYDOWN:
                switch (e.key.keysym.sym)
                {
                    case SDLK_1:
                        for (int i = 0; i < 10 && !allBoids.empty(); ++i)
                        {
                            delete allBoids.back();
                            allBoids.pop_back();
                        }
                        break;
                    case SDLK_2:
                        if (!allBoids.empty())
                        {
                            delete allBoids.back();
                            allBoids.pop_back();
                        }
                        break;
                    case SDLK_3:
                        MakeBoidType1();
                        break;
                    case SDLK_4:
                        for (int i = 0; i < std::floor(10 * RATIO); ++i)
                            MakeBoidType1();

                        for (int i = 0; i < std::ceil(10 * (1 - RATIO)); ++i)
                            MakeBoidType2();
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
    for (auto b : allBoids)
        delete b;
}

