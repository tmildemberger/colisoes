#include "sdl_interface.hpp"

#include <iostream>
#include <algorithm>

// Sdl //
Sdl::Sdl(Uint32 flags) {
    if (SDL_Init(flags) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " <<
                        SDL_GetError() << '\n';
        throw -1;
    }
    int imgFlags { IMG_INIT_PNG };
    if (!(IMG_Init(imgFlags) & imgFlags)) {
        std::cerr << "SDL_image could not initialize! SDL_image_Error: " <<
                        IMG_GetError() << '\n';
        throw -1;
    }
}

Sdl::~Sdl() {
    IMG_Quit();
    SDL_Quit();
}

void Sdl::setAntiAliasing(const char* option) {
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, option);
}

void Sdl::handleEvents() {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_WINDOWEVENT) {
            if (e.window.event == SDL_WINDOWEVENT_CLOSE) {
                for (auto& w : windows) {
                    if (w.id == e.window.windowID) {
                        w.setWindowShouldClose(true);
                        break;
                    }
                }
            }
        } else if (e.type == SDL_KEYDOWN) {
            switch (e.key.keysym.scancode) {
                case SDL_SCANCODE_SPACE:
                    if (onSpaceDown) {
                        onSpaceDown();
                    }
                    break;
                case SDL_SCANCODE_ESCAPE:
                    if (onEscapeDown) {
                        onEscapeDown();
                    }
                    break;
                default:
                    break;
            }
        }
    }
    windows.erase(
        std::remove_if(windows.begin(), windows.end(), 
                       [=] (const Sdl::Window& win) { 
                           return win.windowShouldClose();
                       } ),
        windows.end());
}

void Sdl::setOnSpaceDownCallback(std::function<void()> callback) {
    onSpaceDown = callback;
}

void Sdl::setOnEscapeDownCallback(std::function<void()> callback) {
    onEscapeDown = callback;
}

std::size_t Sdl::openWindows() const {
    return windows.size();
}

Sdl::Window&
Sdl::createWindow(const std::string& title, int x, int y,
                                            int w, int h, Uint32 flags) {
    windows.push_back(Sdl::Window { title, w, h, x, y, flags });
    return windows.back();
}
// Sdl //

// Window //
Sdl::Window::Window(Window&& other)
    : renderer (std::move(other.renderer)) {
    window = other.window;
    other.window = nullptr;
    id = other.id;
    other.id = 0;
    shouldClose = other.shouldClose;
}

Sdl::Window& Sdl::Window::operator=(Sdl::Window&& other) {
    window = other.window;
    other.window = nullptr;
    renderer = std::move(other.renderer);
    id = other.id;
    other.id = 0;
    shouldClose = other.shouldClose;
    return *this;
}

Sdl::Window::Window(const std::string& title, int x, int y,
                                              int w, int h, Uint32 flags)
    : window (SDL_CreateWindow(title.c_str(), x, y, w, h, flags)) {
    if (!window) {
        std::cerr << "Window could not be created! SDL_Error: " <<
                        SDL_GetError() << '\n';
        throw -1;
    }
    // renderer = std::make_unique<Sdl::Renderer>(window); não deu 
    renderer = std::unique_ptr<Sdl::Renderer>(new Sdl::Renderer { window });
    id = SDL_GetWindowID(window);
    // shouldClose = false
}

Sdl::Window::~Window() {
    if (window) {
        SDL_DestroyWindow(window);
    }
}

SDL_PixelFormat* Sdl::Window::getFormat() const {
    return SDL_GetWindowSurface(window)->format;
}

SDL_Window* Sdl::Window::getWindow() const {
    return window;
}

Sdl::Renderer& Sdl::Window::getRenderer() {
    return *renderer;
}

void Sdl::Window::update() const {
    SDL_UpdateWindowSurface(window);
}

bool Sdl::Window::windowShouldClose() const {
    return shouldClose;
}

void Sdl::Window::setWindowShouldClose(bool should) {
    shouldClose = should;
}
// Window //

// Renderer //
Sdl::Renderer::Renderer(SDL_Window* window, Uint32 flags)
    : renderer (SDL_CreateRenderer(window, -1, flags)) {
    if (!renderer) {
        std::cerr << "Renderer could not be created! SDL_Error: " <<
                        SDL_GetError() << '\n';
        throw -1;
    }
}

Sdl::Renderer::Renderer(Renderer&& other) {
    renderer = other.renderer;
    other.renderer = nullptr;
}

Sdl::Renderer& Sdl::Renderer::operator=(Sdl::Renderer&& other) {
    renderer = other.renderer;
    other.renderer = nullptr;
    return *this;
}

Sdl::Renderer::~Renderer() {
    SDL_DestroyRenderer(renderer);
}

SDL_Renderer* Sdl::Renderer::getRenderer() const {
    return renderer;
}

Uint32 Sdl::Renderer::getPixelFormat() const {
    SDL_RendererInfo info;
    SDL_GetRendererInfo(renderer, &info);
    return info.texture_formats[0];
}

void Sdl::Renderer::setDrawColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a) const {
    SDL_SetRenderDrawColor(renderer, r, g, b, a);
}

void Sdl::Renderer::drawPoint(int x, int y) const {
    SDL_RenderDrawPoint(renderer, x, y);
}

void Sdl::Renderer::drawLine(int x1, int y1, int x2, int y2) const {
    SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
}

void Sdl::Renderer::drawRect(SDL_Rect rect) const {
    SDL_RenderDrawRect(renderer, &rect);
}

void Sdl::Renderer::fillRect(SDL_Rect rect) const {
    SDL_RenderFillRect(renderer, &rect);
}

void Sdl::Renderer::clear() const {
    SDL_RenderClear(renderer);
}

void Sdl::Renderer::swap() const {
    SDL_RenderPresent(renderer);
}

void Sdl::Renderer::setViewport(SDL_Rect rect) const {
    SDL_RenderSetViewport(renderer, &rect);
}

void Sdl::Renderer::copy(const Texture& texture) const {
    SDL_RenderCopy(renderer, texture.getTexture(), nullptr, nullptr);
}

void Sdl::Renderer::copy(const Texture& texture, SDL_Rect dst) const {
    SDL_RenderCopy(renderer, texture.getTexture(), nullptr , &dst);
}

void Sdl::Renderer::copy(const Texture& texture, SDL_Rect src,
                                                 SDL_Rect dst) const {
    SDL_RenderCopy(renderer, texture.getTexture(), &src, &dst);
}
// Renderer //

// Texture //
Sdl::Texture::Texture(const Renderer& renderer, int w, int h)
    : texture (SDL_CreateTexture(renderer.getRenderer(),
                                 renderer.getPixelFormat(),
                                 SDL_TEXTUREACCESS_TARGET,
                                 w, h)) {
    if (!texture) {
        std::cerr << "Unable to create texture! SDL_Error: " <<
                      SDL_GetError() << '\n';
        throw -1;
    }
}

Sdl::Texture::Texture(const Renderer& renderer, const std::string& image) {
    SDL_Surface* surface { IMG_Load(image.c_str()) };
    if (!surface) {
        std::cerr << "Unable to load image " << image <<
                        "! IMG_Error: " << IMG_GetError() << '\n';
        throw -1;
    }
    texture = SDL_CreateTextureFromSurface(renderer.getRenderer(), surface);
    if (!texture) {
        std::cerr << "Unable to create texture from " << image <<
                        "! SDL_Error: " << SDL_GetError() << '\n';
        throw -1;
    }
}

Sdl::Texture::~Texture() {
    SDL_DestroyTexture(texture);
}

SDL_Texture* Sdl::Texture::getTexture() const {
    return texture;
}
// Texture //