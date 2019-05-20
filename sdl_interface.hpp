#ifndef SDL_INTERFACE_HPP
#define SDL_INTERFACE_HPP

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string>
#include <vector>
#include <memory>
#include <functional>

class Sdl {
public:
    Sdl(Uint32 flags);
    void setAntiAliasing(const char* option);
    ~Sdl();

    void handleEvents();
    void setOnSpaceDownCallback(std::function<void()> callback);
    void setOnEscapeDownCallback(std::function<void()> callback);
    std::size_t openWindows() const;

    class Window;
    class Renderer;
    class Texture;

    Window& createWindow(const std::string& title, int w, int h,
                         int x = SDL_WINDOWPOS_UNDEFINED,
                         int y = SDL_WINDOWPOS_UNDEFINED,
                         Uint32 flags = SDL_WINDOW_SHOWN);
private:
    std::vector<Window> windows;
    std::function<void()> onSpaceDown;
    std::function<void()> onEscapeDown;
};

class Sdl::Renderer {
public:
    friend class Sdl::Window;
    Renderer(const Renderer&) = delete;
    Renderer(Renderer&&);
    Renderer& operator=(Renderer&&);
    ~Renderer();
    SDL_Renderer* getRenderer() const;
    Uint32 getPixelFormat() const;
    void setDrawColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a) const;
    void drawPoint(int x, int y) const;
    void drawLine(int x1, int y1, int x2, int y2) const;
    void drawRect(SDL_Rect rect) const;
    void fillRect(SDL_Rect rect) const;
    void clear() const;
    void swap() const;
    void setViewport(SDL_Rect rect) const;
    void copy(const Sdl::Texture& texture) const;
    void copy(const Sdl::Texture& texture, SDL_Rect dst) const;
    void copy(const Sdl::Texture& texture, SDL_Rect src, SDL_Rect dst) const;
private:
    Renderer(SDL_Window* window, Uint32 flags = SDL_RENDERER_ACCELERATED |
                                                SDL_RENDERER_TARGETTEXTURE);
    SDL_Renderer* renderer { nullptr };
};

class Sdl::Window {
public:
    friend class Sdl;
    Window(const Window&) = delete;
    Window(Window&&);
    Window& operator=(Window&&);

    SDL_PixelFormat* getFormat() const;
    SDL_Window* getWindow() const;
    Sdl::Renderer& getRenderer();
    void update() const;
    bool windowShouldClose() const;
    void setWindowShouldClose(bool should);
    ~Window();
private:
    Window(const std::string& title, int w, int h, 
                                     int x = SDL_WINDOWPOS_UNDEFINED,
                                     int y = SDL_WINDOWPOS_UNDEFINED,
                                     Uint32 flags = SDL_WINDOW_SHOWN);
    SDL_Window* window { nullptr };
    std::unique_ptr<Sdl::Renderer> renderer;
    Uint32 id { 0 };
    bool shouldClose { false };
};

class Sdl::Texture {
public:
    Texture(const Texture&) = delete;
    Texture(const Sdl::Renderer& renderer, int w, int h);
    Texture(const Sdl::Renderer& renderer, const std::string& image);
    ~Texture();
    SDL_Texture* getTexture() const;
private:
    SDL_Texture* texture { nullptr };
};

#endif // SDL_INTERFACE_HPP