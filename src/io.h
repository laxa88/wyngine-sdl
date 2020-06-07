#include <SDL2/SDL.h>
#include <string>

struct WY_Text
{
    std::string text;
    std::string composition; // reserved for inputs like Asian characters
    Sint32 cursor;
    Sint32 selection_len;
};

class WY_IO
{
    WY_Text txt{"", "", 0, 0};

public:
    WY_IO() {}

    std::string getText()
    {
        return txt.text.c_str();
    }

    int getCursor()
    {
        return txt.cursor;
    }

    void update(SDL_Event *windowEvent, int hasEvent)
    {
        if (hasEvent == 0)
        {
            return;
        }

        int keycode = windowEvent->key.keysym.sym;
        int type = windowEvent->type;

        switch (type)
        {
        case SDL_KEYDOWN:
        {
            switch (keycode)
            {
            case SDLK_BACKSPACE:
                if (txt.text.length() > 0)
                {
                    txt.text.pop_back();
                }
                break;

            case SDLK_RETURN:
            case SDLK_RETURN2:
                txt.text.append("\n");
                break;
            }
        }
        break;

        case SDL_TEXTINPUT:
            txt.text.append(windowEvent->text.text);
            break;

        case SDL_TEXTEDITING:
            txt.composition = windowEvent->edit.text;
            txt.cursor = windowEvent->edit.start;
            txt.selection_len = windowEvent->edit.length;
            break;
        }
    }
};
