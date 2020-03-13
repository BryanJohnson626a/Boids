// Boids
// Bryan Johnson

#include <chrono>
#include <thread>

#include "Graphics.h"
#include "../GameLoop.h"

const int FPS_TARGET = 30;
const int FRAME_MS = 1000 / FPS_TARGET;

typedef std::chrono::duration<int, std::milli> millisecs_t;

int main(int args, char * argv[])
{
    std::vector<std::string> cmd_args;
    cmd_args.reserve(static_cast<unsigned long>(args));
    for (int i = 0; i < args; ++i)
        cmd_args.emplace_back(argv[i]);

    PE::Graphics graphics;
    graphics.Initialize();
    bool running = true;
    std::chrono::steady_clock::time_point prev_time = std::chrono::steady_clock::now();
    std::chrono::steady_clock::time_point cur_time;
    float dt = 0;

    GameInit(cmd_args);

    while (running)
    {
        cur_time = std::chrono::steady_clock::now();
        millisecs_t duration(std::chrono::duration_cast<millisecs_t>(cur_time - prev_time));
        int ms_elapsed = duration.count();
        if (ms_elapsed < FRAME_MS)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(FRAME_MS - ms_elapsed));
            continue;
        }
        prev_time = cur_time;
        dt = ms_elapsed / 1000.0f;

        running = GameLoop(dt);

        graphics.Update(dt);
    }

    GameShutdown();
    graphics.Deinit();

    return 0;
}
