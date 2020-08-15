// Created by bushk on 8/15/2020.

#pragma once

class BoidController;
class GameUI
{
public:
    static void SetInstance(GameUI * game_ui);
    static GameUI * GetGameUI();
    void UpdateGameUI();
    BoidController * bc1;
    BoidController * bc2;
private:
    static GameUI * instance;
};



