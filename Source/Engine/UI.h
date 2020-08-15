// Created by Bryan Johnson on 8/14/2020.
#pragma once

#include <iostream>
#include "SDL2/SDL.h"
#include "imgui.h"

namespace PE
{
  void SetupImgui(SDL_Window *window, SDL_GLContext gl_context);
  void UpdateImgui(SDL_Window *window);
  void CleanupImgui();
}