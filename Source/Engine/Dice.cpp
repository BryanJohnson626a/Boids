//
// Created by bryan on 2/23/20.
//

#include <chrono>

#include "Dice.h"

RNG Die::generator(static_cast<unsigned long>(std::chrono::system_clock::now().time_since_epoch().count()));
RNG DieReal::generator(static_cast<unsigned long>(std::chrono::system_clock::now().time_since_epoch().count()));

Die::Die(unsigned high) : Die(1, static_cast<int>(high))
{}

Die::Die(int low, int high) : roller(low, high), Low(low), High(high)
{}

void Die::SetSeed(unsigned seed)
{
    generator = RNG(seed);
}

int Die::Roll()
{
    return roller(generator);
}

int Die::Roll(int times)
{
    return roller(generator);
}

DieReal::DieReal(float low, float high) : roller(low, high), Low(low), High(high)
{}

void DieReal::SetSeed(unsigned seed)
{
    generator = RNG(seed);
}

float DieReal::Roll()
{
    return roller(generator);
}

float DieReal::Roll(int times)
{
    return roller(generator);
}

float DieReal::Roll(float low, float high)
{
    auto die = std::uniform_real_distribution<float>(low, high);
    return die(generator);
}
