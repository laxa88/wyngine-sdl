// https://lazyfoo.net/tutorials/SDL/41_bitmap_fonts/index.php

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string>

class WY_MonoFont
{
    SDL_Texture *mTexture = nullptr;
    SDL_Rect mBound{0, 0, 0, 0};
    SDL_Rect mDest{0, 0, 0, 0};
    SDL_Rect mChars[256];
    int mFontSize; // monospace fonts assumed to be equal w/h
    int mPaddingV; // vertical padding between lines
    int mCurrX, mCurrY;
    bool mDebug;

    void buildFonts()
    {
        int texW, texH;
        SDL_QueryTexture(mTexture, NULL, NULL, &texW, &texH);

        // Assumes font spritesheet uses ascii layout
        for (int i = 0; i < 256; i++)
        {
            mChars[i] = {
                (i * mFontSize) % texW,
                (i * mFontSize / texW) * mFontSize,
                mFontSize,
                mFontSize};
        }
    }

    int getFullAscii(std::string text, int index)
    {
        if (index < 0 || index >= text.length())
        {
            return (unsigned char)' ';
        }

        return (unsigned char)text[index];
    }

    int getMiniAscii(std::string text, int index)
    {
        if (index < 0 || index >= text.length())
        {
            return (unsigned char)' ';
        }

        // mini-ascii offsets by first 32 unused glyphs
        return (unsigned char)text[index] - 32;
    }

public:
    int getW()
    {
        return mBound.w;
    }

    void setW(int w)
    {
        mBound.w = w;
    }

    void setDebug(bool flag)
    {
        mDebug = flag;
    }

    WY_MonoFont(SDL_Texture *t, int fs, int vp, SDL_Rect b)
    {
        mTexture = t;
        mFontSize = fs;
        mPaddingV = vp;
        mBound = b;
        mDebug = false;

        buildFonts();
    }

    WY_MonoFont(SDL_Texture *t, int fs, int vp) : WY_MonoFont(t, fs, vp, {0, 0, 0, 0}) {}
    WY_MonoFont(SDL_Texture *t, int fs) : WY_MonoFont(t, fs, 4, {0, 0, 0, 0}) {}
    WY_MonoFont(SDL_Texture *t) : WY_MonoFont(t, 8, 4, {0, 0, 0, 0}) {}

    ~WY_MonoFont()
    {
        mTexture = nullptr;
        delete &mBound;
        delete &mDest;
        delete[] & mChars;
    }

    void printm(SDL_Renderer *renderer, const char *texts[], int length)
    {
        // https://stackoverflow.com/questions/33523585/how-do-sizeofarr-sizeofarr0-work
        // array decays to a pointer when passed into function,
        // so it's impossible to get array length.

        std::string str = "";

        for (int i = 0; i < length; i++)
        {
            str += texts[i];
        }

        print(renderer, str);
    }

    void print(SDL_Renderer *renderer, std::string text)
    {
        if (mTexture == nullptr)
        {
            SDL_Log("Font texture missing!\n");
            return;
        }

        if (mDebug)
        {
            SDL_SetRenderDrawColor(renderer, 0x00, 0xFF, 0x00, 0xFF); // green
            SDL_RenderDrawRect(renderer, &mBound);
        }

        mCurrX = mBound.x;
        mCurrY = mBound.y;

        for (int c = 0; c < text.length(); c++)
        {
            // pre-trim whitespaces from first word in the line
            int fullAscii = getFullAscii(text, c);
            while (mCurrX == mBound.x && fullAscii == ' ')
            {
                c++;
                fullAscii = getFullAscii(text, c);
            }

            // if we encounter a linebreak, break and start over
            if (fullAscii == '\n')
            {
                mCurrX = mBound.x;
                mCurrY += (mPaddingV + mFontSize);
                continue;
            }

            // get end index of word
            int endPos = c + 1;
            int endPosFullAscii = getFullAscii(text, endPos);
            while (endPosFullAscii != ' ' && endPosFullAscii != '\n')
            {
                endPos++;
                endPosFullAscii = getFullAscii(text, endPos);
            }
            int currFullWidth = mCurrX + ((endPos - c) * mFontSize);

            // if word is not first in line and exceeds bound width, break to new line
            if (mCurrX != mBound.x && currFullWidth > mBound.x + mBound.w)
            {
                mCurrX = mBound.x;
                mCurrY += (mPaddingV + mFontSize);
                continue;
            }

            for (; c < endPos; c++)
            {
                int miniAscii = getMiniAscii(text, c);
                mDest = {mCurrX, mCurrY, mFontSize, mFontSize};
                SDL_RenderCopy(renderer, mTexture, &mChars[miniAscii], &mDest);

                mCurrX += mFontSize;
            }

            c--;
        }

        // IMPROVEMENTS
        // - colorize font
        // - animate to appear letter by letter
        // - animate each character with wobbly effect
        // - add custom animation per character
    }
};
