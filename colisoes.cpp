#include <iostream>
#include <string>
#include <SDL2/SDL.h>

/**
 * Essa classe existe porque RAII
*/
class SDLrename {
public:
    SDLrename(Uint32 flags) {
        if (SDL_Init(flags) < 0) {
            std::cout << "SDL could not initialize! SDL_Error: " <<
                          SDL_GetError() << '\n';
            throw -1;
        }
    }
    ~SDLrename() {
        SDL_Quit();
    }
private:

};

class SDLwinaff {
public:
    SDLwinaff(const std::string& title, int w, int h)
        : SDLwinaff(title, w, h, SDL_WINDOW_SHOWN) {
        
    }
    SDLwinaff(const std::string& title, int w, int h, Uint32 flags) 
        : SDLwinaff(title, SDL_WINDOWPOS_UNDEFINED,
                           SDL_WINDOWPOS_UNDEFINED, w, h, flags) {
        
    }
    SDLwinaff(const std::string& title, int x, int y,
                                        int w, int h, Uint32 flags)
        : window(SDL_CreateWindow(title.c_str(), x, y, w, h, flags)) {
        if (!window) {
            std::cout << "Window could not be created! SDL_Error: " <<
                        SDL_GetError() << '\n';
            throw -1;
        }
    }
    ~SDLwinaff() {
        SDL_DestroyWindow(window);
    }
    SDL_Surface* getSurface() {
        return SDL_GetWindowSurface(window);
    }
    void update() {
        SDL_UpdateWindowSurface(window);
    }
private:
    SDL_Window* window;
};

class SDLimageflash {
public:
    SDLimageflash(const std::string& path_to_image) 
        : image(SDL_LoadBMP(path_to_image.c_str())) {
        if (!image) {
            std::cout << "Unable to load image " << path_to_image <<
                         "! SDL_Error: " << SDL_GetError() << '\n';
            throw -1;
        }
    }
    ~SDLimageflash() {
        SDL_FreeSurface(image);
    }
    SDL_Surface* getSurface() {
        return image;
    }
private:
    SDL_Surface* image;
};

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[]) {
    constexpr int screen_width { 640 };
    constexpr int screen_height { 480 };

    // SDL_Window* window { nullptr };
    SDL_Surface* screenSurface { nullptr };

    // if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    //     std::cout << "SDL could not initialize! SDL_Error: " <<
    //                   SDL_GetError() << '\n';
    //     return -1;
    // }
    SDLrename context { SDL_INIT_VIDEO };
    SDLwinaff window { "SDL tutorial", screen_width, screen_height };

    // window = SDL_CreateWindow("SDL tutorial",
    //                           SDL_WINDOWPOS_UNDEFINED,
    //                           SDL_WINDOWPOS_UNDEFINED,
    //                           screen_width,
    //                           screen_height,
    //                           SDL_WINDOW_SHOWN);
    // if (window == nullptr) {
    //     std::cout << "Window could not be created! SDL_Error: " <<
    //                   SDL_GetError() << '\n';
    //     SDL_Quit();
    //     return -1;
    // }
    //Get window surface
    screenSurface = window.getSurface();

    //Fill the surface white
    // SDL_FillRect(screenSurface,
    //              nullptr,
    //              SDL_MapRGB(screenSurface->format, 0xff, 0xff, 0xff));
    SDLimageflash image { "x.bmp" };

    SDL_BlitSurface(image.getSurface(), nullptr, screenSurface, nullptr);

    //Update the surface
    // SDL_UpdateWindowSurface(window);
    window.update();

    SDL_Delay(2000);

    // SDL_DestroyWindow(window);
    // SDL_Quit();

    return 0;
}