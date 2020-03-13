//
// Created by bryan on 2/23/20.
//

#pragma once

#include <random>

using RNG = std::mt19937_64;

class Die
{
public:
    /**
     * Creates a die that rolls between 1 and high.
     * @param high The highest result possible.
     */
    explicit Die(unsigned high);

    /**
     * Creates a die that rolls between two values.
     * @param low The lowest result possible.
     * @param high The highest result possible.
     */
    Die(int low, int high);

    /**
     * Sets the seed used by all dice.
     * @param seed
     */
    static void SetSeed(unsigned seed);

    /**
     * Roll the die once.
     * @return The result of the die roll.
     */
    [[nodiscard]] int Roll();

    /**
     * Roll the die multiple times.
     * @param times How many times to roll.
     * @return The total of all dice rolled.
     */
    [[nodiscard]] int Roll(int times);

    const int Low, High;
private:
    static RNG generator;
    std::uniform_int_distribution<int> roller;
};

class DieReal
{
public:
    /**
     * Creates a die that rolls between two values.
     * @param low The lowest result possible.
     * @param high The highest result possible.
     */
    DieReal(float low, float high);

    /**
     * Sets the seed used by all dice.
     * @param seed
     */
    static void SetSeed(unsigned seed);

    /**
     * Roll the die once.
     * @return The result of the die roll.
     */
    [[nodiscard]] float Roll();

    /**
     * Roll the die multiple times.
     * @param times How many times to roll.
     * @return The total of all dice rolled.
     */
    [[nodiscard]] float Roll(int times);

    static float Roll(float low, float high);

    const float Low, High;
private:
    static RNG generator;
    std::uniform_real_distribution<float> roller;
};
