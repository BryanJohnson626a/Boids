// Created by bushk on 8/15/2020.

#pragma once

#include <vector>

class BoidController;
class GameUI
{
public:
    static void SetInstance(GameUI * game_ui);
    static GameUI * GetGameUI();
    void UpdateGameUI();
    std::vector<BoidController *> BoidControllers;
private:
    static GameUI * instance;
};



