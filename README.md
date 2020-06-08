# Wyngine (SDL2)

Yet another C++ game framework using SDL2. The emphasis of this framework is minimal tools to make simple retro-looking 2D games for desktop and web targets.

## Features

### Base

v1.0

- [x] Loads PNG images
- [x] Supports mini-ASCII bitmap font
- [ ] Audio synthesis
- [x] Keyboard input
- [ ] Mouse/touch input
- [x] Targets Windows (desktop) via SDL2
- [x] Targets web (HTML5) via emscripten/SDL2

v2.0

- [ ] Image rotation
- [ ] Sprite animation

### Nice to have

- [x] FPS limit
- [ ] Binary icon
- [ ] Mac OSX dmg packager
- [ ] Per-character bitmap font manipulation
- [ ] Save data
- [ ] Network request (GET/POST)

## Demos

- [x] Timer / FPS
- [x] Virtual keyboard
- [ ] Audio
- [ ] Pixel gradient animation
- [ ] Bunnymark

## Setup

### Windows Desktop

Check `.vscode` and `Makefile` settings, modify accordingly to setup your build workflow.

- In your current active window (e.g. `main.cpp`), press `CTRL + SHIFT + B` to build.
- Press `F5` to build and debug.
- Include `libpng16-16.dll` in your output folder.

### Mac OSX

- Use brew: `brew update` > `brew upgrade`
- Install GCC: `brew install gcc`
- Install SDL2: `brew install sdl2`
- Install SDL2 image: `brew install sdl2_image`
- Build: `g++ main.cpp -std=c++11 -o main -L/usr/local/lib -lSDL2 -lSDL2_image`

Note: Use `brew` to take advantage of the default directory, i.e. `/usr/local/lib`

### Web / HTML5 (Windows)

- Install emscripten
- Open `emcmdprompt.bat` (which contains `emcc` command) from installation folder
- Build: `emcc main.cpp -O3 -s USE_SDL=2 -s USE_SDL_IMAGE=2 --preload-file img -o bin-js/webgame.html`

### Web / HTML5 (Mac OSX)

todo

## Deployment

### Windows Desktop

todo

### Mac OSX

todo

### Web / HTML5

todo

## License

Just throwing this here, copy-pasted from [here](https://opensource.org/licenses/MIT).

> Copyright 2020 LWY
>
> Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
>
> The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
>
> THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
