// Prototype code

#include <vector>

#include "Boid.h"
#include "Behavior.h"
#include "GameLoop.h"
#include "Engine/Dice.h"
#include "Engine/Graphics.h"

const int DEFAULT_NUM_BOIDS = 1000;
const float RATIO = 0.9f;

std::vector<Boid *> allBoids;
std::vector<Boid *> boids1;
std::vector<Boid *> boids2;

std::vector<PE::Vertex> bverts{
        PE::Vertex{PE::Vec3{1, 0, 0}, PE::Vec3{0, 0, 1}, PE::Vec2{0.5, 1}},
        PE::Vertex{PE::Vec3{-1, -1, 0}, PE::Vec3{0, 0, 1}, PE::Vec2{0.5, 1}},
        PE::Vertex{PE::Vec3{-0.5, 0, 0}, PE::Vec3{0, 0, 1}, PE::Vec2{0.5, 1}},
        PE::Vertex{PE::Vec3{-1, 1, 0}, PE::Vec3{0, 0, 1}, PE::Vec2{0.5, 1}}
};
std::vector<uint> bindices{
        0, 1, 2,
        0, 2, 3
};
PE::TextureList btex;

static PE::Model * BoidModel;

void MakeBoidType1()
{
    static DieReal AvoidFac(0.25f, 0.5f);
    static DieReal AvoidDis(0.25f, 1.5f);
    static DieReal AlignFac(1.f, 1.5f);
    static DieReal AlignDis(1.f, 1.5f);
    static DieReal CohesFac(0.5f, 1.5f);
    static DieReal CohesDis(1.5f, 2.5f);
    static DieReal FearFac(1.f, 1.5f);
    static DieReal FearDis(2.2f, 3.3f);

    auto * b = new Boid(*BoidModel);
    allBoids.push_back(b);
    boids1.push_back(b);

    b->AddBehavior(new BehaviorAvoid(b, AvoidFac.Roll(), AvoidDis.Roll(), &boids1));
    b->AddBehavior(new BehaviorAlign(b, AlignFac.Roll(), AlignDis.Roll(), &boids1));
    b->AddBehavior(new BehaviorCohesion(b, CohesFac.Roll(), CohesDis.Roll(), &boids1));
    b->AddBehavior(new BehaviorAvoid(b, FearFac.Roll(), FearDis.Roll(), &boids2));

    b->Scale(0.75f);
    b->SetMaterial(PE::pearl);
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

    Boid * b = new Boid(*BoidModel);
    allBoids.push_back(b);
    boids2.push_back(b);

    b->AddBehavior(new BehaviorAvoid(b, AvoidFac.Roll(), AvoidDis.Roll(), &boids2));
    b->AddBehavior(new BehaviorAlign(b, AlignFac.Roll(), AlignDis.Roll(), &boids1));
    b->AddBehavior(new BehaviorCohesion(b, CohesFac.Roll(), CohesDis.Roll(), &boids1));

    b->Scale(1.25f);
    b->SetMaterial(PE::ruby);
    b->SetSpeed(0.75f);
}

void GameInit(std::vector<std::string> cmd_args)
{
    PE::Mesh BoidMesh = PE::Mesh(bverts, bindices, btex);
    BoidModel = new PE::Model(BoidMesh);

    int num_boid1, num_boid2;

    if (cmd_args.size() == 2)
    {
        int total = std::stoi(cmd_args[1]);
        num_boid1 = std::floor((float) total * RATIO);
        num_boid2 = std::ceil((float) total * (1 - RATIO));
    }
    else if (cmd_args.size() == 3)
    {
        num_boid1 = std::stoi(cmd_args[1]);
        num_boid2 = std::stoi(cmd_args[2]);
    }
    else
    {
        num_boid1 = std::floor(DEFAULT_NUM_BOIDS * RATIO);
        num_boid2 = std::ceil(DEFAULT_NUM_BOIDS * (1 - RATIO));
    }

    for (int i = 0; i < num_boid1; ++i)
        MakeBoidType1();

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
                }
                break;
            case SDL_QUIT:
                return false;
        }
    }

    return true;
}

void GameShutdown()
{
    for (auto b : allBoids)
        delete b;
}

