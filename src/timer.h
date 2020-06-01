#include <SDL2/SDL_timer.h>

class WY_Timer
{
    int startTime;
    int frames;
    int dTime1;
    int dTime2;

public:
    WY_Timer()
    {
        reset();
    }

    // actual time elapsed
    int getTimeSinceStart()
    {
        return SDL_GetTicks();
    }

    int getStartTime()
    {
        return SDL_GetTicks() - startTime;
    }

    // number of updates called
    int getFrames()
    {
        return frames;
    }

    // delta time in milliseconds
    int getDeltaTime()
    {
        return dTime2 - dTime1;
    }

    int getFPS()
    {
        float fps = getFrames() / (getStartTime() / 1000.f);
        if (fps > 9999)
        {
            fps = 0;
        }

        return fps;
    }

    void reset()
    {
        frames = 0;
        startTime = SDL_GetTicks();
        dTime1 = SDL_GetTicks();
        dTime2 = SDL_GetTicks();
    }

    void update()
    {
        frames++;
        dTime1 = dTime2;
        dTime2 = SDL_GetTicks();
    }
};