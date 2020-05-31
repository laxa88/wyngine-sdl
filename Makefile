# Note: Don't optimize with -O when debugging; It screws breakpoints
# run `mingw32-make` to invoke "all" rule
# run `mingw32-make <rule-name>` to invoke specific rule

# web build (-03 flag helps!)
# emcc main.c -O3 -s USE_SDL=2 -o bin-js/webgame.html
# emcc main2.cpp -O3 -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s SDL2_IMAGE_FORMATS=["png"] --preload-file assets -o bin-js/webgame.html

all : \
	audio-demo

audio-demo:
	g++ -g audio-demo.cpp \
	-IC:\wy-dev\sdl2-mingw-32\include \
	-IC:\wy-dev\sdl2-mingw-32\include\SDL2 \
	-IC:\wy-dev\SDL2_image-2.0.5\i686-w64-mingw32\include \
	-LC:\wy-dev\sdl2-mingw-32\lib \
	-LC:\wy-dev\sdl2-mingw-32\lib\SDL2 \
	-LC:\wy-dev\SDL2_image-2.0.5\i686-w64-mingw32\lib \
	-lmingw32 -lSDL2main -lSDL2 -lSDL2_image  \
	-lgdi32 -lwinmm -limm32 -lole32 -loleaut32 -lsetupapi -lversion \
	-static \
	-o bin\audio-demo
