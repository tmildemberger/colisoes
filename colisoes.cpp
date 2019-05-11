#include <iostream>
#include <SDL2/SDL.h>


int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[]) {
    constexpr int screen_width { 640 };
    constexpr int screen_height { 480 };

    SDL_Window* window { nullptr };
    SDL_Surface* screenSurface { nullptr };

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << "SDL could not initialize! SDL_Error: " <<
                      SDL_GetError() << '\n';
        return -1;
    }
    window = SDL_CreateWindow("SDL tutorial",
                              SDL_WINDOWPOS_UNDEFINED,
                              SDL_WINDOWPOS_UNDEFINED,
                              screen_width,
                              screen_height,
                              SDL_WINDOW_SHOWN);
    if (window == nullptr) {
        std::cout << "Window could not be created! SDL_Error: " <<
                      SDL_GetError() << '\n';
        SDL_Quit();
        return -1;
    }
    //Get window surface
    screenSurface = SDL_GetWindowSurface(window);

    //Fill the surface white
    SDL_FillRect(screenSurface,
                 nullptr,
                 SDL_MapRGB(screenSurface->format, 0xff, 0xff, 0xff));

    //Update the surface
    SDL_UpdateWindowSurface(window);

    SDL_Delay(2000);

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}