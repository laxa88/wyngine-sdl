#include <SDL2/SDL_timer.h>

class WY_Timer
{
    int startTime;
    int frames;
    int dTime1;
    int dTime2;
    int fpsLimit; // user-defined FPS limit, e.g. 60
    int fps;      // calculated rate per fps, e.g. 0.016 (at 60fps)

public:
    WY_Timer(int f)
    {
        setFPS(f);
        reset();
    }

    // actual time elapsed
    int getTimeSinceStart()
    {
        return SDL_GetTicks();
    }

    // time since last reset
    int getStartTime()
    {
        return SDL_GetTicks() - startTime;
    }

    // number of updates called
    int getFrames()
    {
        return frames;
    }

    // time difference in ms between start of current frame and start of previous frame
    int getDeltaTime()
    {
        return dTime2 - dTime1;
    }

    int getCurrFPS()
    {
        if (frames <= 0 || getStartTime() <= 0)
        {
            return 0;
        }

        float fps = frames / (getStartTime() / 1000.f);
        if (fps > 9999)
        {
            return 0;
        }

        return fps;
    }

    int getFPS()
    {
        return fpsLimit;
    }

    void setFPS(int f)
    {
        fpsLimit = f;
        fps = 1000 / fpsLimit;
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
        dTime1 = dTime2;
        dTime2 = SDL_GetTicks();
    }

    void delayByFPS()
    {
        frames++;

        int dt = SDL_GetTicks() - dTime2;

        if (dt < fps)
        {
            SDL_Delay(fps - dt);
        }
    }
};