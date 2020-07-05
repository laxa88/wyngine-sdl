#include <SDL2/SDL.h>

#include "../src/wyngine.h"
#include "../src/math.h"
#include "../src/font.h"

#define WIDTH 760
#define HEIGHT 600

struct Bunny
{
    int nSpriteIndex;
    double dGravity;
    double dx;
    double dy;
    double dSpeedX;
    double dSpeedY;
    SDL_FRect frectBounds;

    Bunny(int textureIndex, SDL_FRect &bounds)
    {
        nSpriteIndex = textureIndex;
        dGravity = 0.75;
        dSpeedX = wyrandom_d(10);
        dSpeedY = wyrandom_d(10) - 5.0;
        frectBounds = bounds;
    }

    void Update()
    {
        dx += dSpeedX;
        dy += dSpeedY;
        dSpeedY += dGravity;

        if (dx > frectBounds.w)
        {
            dSpeedX *= -1;
            dx = frectBounds.w;
        }
        else if (dx < frectBounds.x)
        {
            dSpeedX *= -1;
            dx = frectBounds.x;
        }

        if (dy > frectBounds.h)
        {
            dSpeedY *= -0.85;
            dy = frectBounds.h;
            if (wyrandom_d(1) > 0.5)
            {
                dSpeedY -= wyrandom_d(6);
            }
        }
        else if (dy < frectBounds.y)
        {
            dSpeedY = 0;
            dy = frectBounds.y;
        }
    }
};

class BunnyMark
{
public:
    WY_Image *imgBunny;
    WY_Sprite srfBunnyTiles;

    std::vector<Bunny> vecBunnies;
    SDL_FRect frectBounds;
    bool bAdding;
    int nCount;
    int nMaxCount;
    int nAmount;

    ~BunnyMark()
    {
        SDL_DestroyTexture(imgBunny->texture);
        delete imgBunny;

        // TODO delete each bunny before clearing?
        vecBunnies.clear();
    }

    BunnyMark(SDL_Renderer *renderer, int startBunnyCount)
    {
        frectBounds.x = 0.0;
        frectBounds.h = HEIGHT;
        frectBounds.w = WIDTH;
        frectBounds.y = 0.0;
        bAdding = false;
        nCount = 0;
        nMaxCount = 200000;
        nAmount = 5;

        imgBunny = loadPNG(renderer, "assets/lineup-fixed.png");
        srfBunnyTiles.renderer = renderer;
        srfBunnyTiles.texture = imgBunny->texture;
        srfBunnyTiles.origin = {0, 0, 35, 36};
        // srfBunnyTiles.SetTextureFilterMode(false, false);
        // srfBunnyTiles.SetTilesetClipping(12, 1);
        nCount = 0;

        if (startBunnyCount > 0)
        {
            AddBunnies(startBunnyCount);
        }
    }

    void Update()
    {
        if (bAdding)
        {
            if (nCount < nMaxCount)
            {
                AddBunnies(nAmount);
            }
        }

        for (auto &bunny : vecBunnies)
        {
            bunny.Update();
        }
    }

    void Draw()
    {
        for (auto &bunny : vecBunnies)
        {
            srfBunnyTiles.Draw({bunny.dx, bunny.dy, 35, 36});

            // srfBunnyTiles.SetTilesetIndex(bunny.nSpriteIndex);
            // srfBunnyTiles.Draw(bunny.dx, bunny.dy);
        }
    }

    void AddBunnies(int count)
    {
        for (int i = 0; i < count; i++)
        {
            Bunny bunny{wyrandom<int>(11), frectBounds};
            bunny.dx = (nCount % 2) * 800.0;
            vecBunnies.push_back(bunny);
            nCount++;
        }
    }

    void StartAdding()
    {
        bAdding = true;
    }

    void StopAdding()
    {
        bAdding = false;
    }

    void Reset()
    {
        // TODO delete each bunny before clearing?

        vecBunnies.clear();
        nCount = 0;
    }
};

class Game : public Wyngine
{
    WY_Image *mFontImage;
    WY_MonoFont *mFont;
    BunnyMark *bunnymark;

    void loadMedia()
    {
        mFontImage = loadPNG(mRenderer, "assets/ascii-bnw.png");
    }

public:
    Game() : Wyngine("Wyngine midi demo", WIDTH, HEIGHT, 1)
    {
        loadMedia();

        mFont = new WY_MonoFont(mFontImage->texture, 8, 4, {8, 8, 240, 208});
        bunnymark = new BunnyMark(mRenderer, 100);
    }

    ~Game()
    {
        SDL_DestroyTexture(mFontImage->texture);
        delete mFontImage;

        delete mFont;
    }

    void onUpdate()
    {
        bunnymark->Update();

        if (keyboard->isKeyDown(SDLK_SPACE))
        {
            bunnymark->StartAdding();
        }
        else if (keyboard->isKeyUp(SDLK_SPACE))
        {
            bunnymark->StopAdding();
        }
    }

    void onRender()
    {
        bunnymark->Draw();

        std::string t1 = "Bunnies : ";
        std::string t2 = std::to_string(bunnymark->nCount);

        mFont->print(mRenderer, t1 + t2);
    }
};

int main(int argc, char *args[])
{
    Game *game = new Game();

    game->run();

    return 0;
}