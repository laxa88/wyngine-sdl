// Reference:
// https://www.falukdevelop.com/2016/08/18/simple-sdl-2-keyboard-key-status/

#include <SDL2/SDL.h>
#include <stdio.h>
#include <string>
#include <map>

class WY_Keyboard
{
    Uint8 prevKeystate[SDL_NUM_SCANCODES];
    Uint8 currKeystate[SDL_NUM_SCANCODES];
    unsigned char charPressed;

public:
    WY_Keyboard()
    {
        memset(prevKeystate, 0, sizeof(Uint8) * SDL_NUM_SCANCODES);
        memcpy(currKeystate, SDL_GetKeyboardState(NULL), sizeof(Uint8) * SDL_NUM_SCANCODES);
    }

    bool isKeyPressed(const SDL_Scancode code)
    {
        return (prevKeystate[code] == 0 && currKeystate[code] == 1);
    }

    bool isKeyReleased(const SDL_Scancode code)
    {
        return (prevKeystate[code] == 1 && currKeystate[code] == 0);
    }

    bool isKeyDown(const SDL_Scancode code)
    {
        return (currKeystate[code] == 1);
    }

    bool isKeyUp(const SDL_Scancode code)
    {
        return (currKeystate[code] == 0);
    }

    bool isAnyKeyDown()
    {
        for (int i = 0; i < SDL_NUM_SCANCODES; i++)
        {
            if (currKeystate[i] == 1)
            {
                return true;
            }
        }

        return false;
    }

    char getLastCharPressed()
    {
        return charPressed;
    }

    void update(SDL_Event *windowEvent)
    {
        if (windowEvent->key.repeat == 1)
        {
            return;
        }

        memcpy(prevKeystate, currKeystate, sizeof(Uint8) * SDL_NUM_SCANCODES);
        memcpy(currKeystate, SDL_GetKeyboardState(NULL), sizeof(Uint8) * SDL_NUM_SCANCODES);

        if (windowEvent->type == SDL_KEYDOWN)
        {
            int keycode = windowEvent->key.keysym.sym;
            SDL_TextInputEvent keytext = windowEvent->text;

            // capitalized chars are handled as input-text in WY_IO
            charPressed = keycode;
        }
    }
};