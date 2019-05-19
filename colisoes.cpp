#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <SDL2/SDL.h>

#include <SDL2/SDL_image.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"
#pragma GCC diagnostic ignored "-Wpedantic"
#include <glm/glm.hpp>
#pragma GCC diagnostic pop

/**
 * Essa classe existe porque RAII
*/
class Sdl {
public:
    Sdl(Uint32 flags);
    void setAntiAliasing(const char* option);
    ~Sdl();

    class Window;
    class Image;
    class Texture;
    class Renderer;

    Window& createWindow(const std::string& title, int w, int h);
    Window& createWindow(const std::string& title, int w, int h, Uint32 flags);
    Window& createWindow(const std::string& title, int x, int y,
                                                   int w, int h, Uint32 flags);
private:
    std::vector<Window> windows;
};

class Sdl::Window {
public:
    friend class Sdl;
    Window(const Window&) = delete;
    Window(Window&&);

    SDL_Surface* getSurface() const;
    SDL_PixelFormat* getFormat() const;
    SDL_Window* getWindow() const;
    void update() const;
    ~Window();
private:
    Window(const std::string& title, int w, int h);
    Window(const std::string& title, int w, int h, Uint32 flags);
    Window(const std::string& title, int x, int y,
                                     int w, int h, Uint32 flags);
    SDL_Window* window;
};

class Sdl::Image {
public:
    Image(const std::string& path_to_image, const SDL_PixelFormat* fmt);
    Image(const std::string& path_to_image);
    ~Image();
    SDL_Surface* getSurface() const;
    std::string getName() const;
private:
    SDL_Surface* image;
    std::string name;
};

class Sdl::Renderer {
public:
    Renderer(const Sdl::Window& window, Uint32 flags);
    ~Renderer();
    SDL_Renderer* getRenderer() const;
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
    SDL_Renderer* renderer;
};

class Sdl::Texture {
public:
    Texture(const Sdl::Renderer& renderer, const Sdl::Image& image);
    ~Texture();
    SDL_Texture* getTexture() const;
private:
    SDL_Texture* texture;
};

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

Sdl::Window&
Sdl::createWindow(const std::string& title, int w, int h) {
    windows.push_back(Sdl::Window { title, w, h, SDL_WINDOW_SHOWN });
    return windows.back();
}

Sdl::Window&
Sdl::createWindow(const std::string& title, int w, int h, Uint32 flags) {
    windows.push_back(Sdl::Window { title, SDL_WINDOWPOS_UNDEFINED,
                                           SDL_WINDOWPOS_UNDEFINED, w, h,
                                           flags });
    return windows.back();
}

Sdl::Window&
Sdl::createWindow(const std::string& title, int x, int y,
                                            int w, int h, Uint32 flags) {
    windows.push_back(Sdl::Window { title, x, y, w, h, flags });
    return windows.back();
}
// Sdl //

// Window //
Sdl::Window::Window(Window&& other) {
    window = other.window;
    other.window = nullptr;
}

Sdl::Window::Window(const std::string& title, int w, int h)
    : Window(title, w, h, SDL_WINDOW_SHOWN) {
    
}

Sdl::Window::Window(const std::string& title, int w, int h, Uint32 flags) 
    : Window(title, SDL_WINDOWPOS_UNDEFINED,
                        SDL_WINDOWPOS_UNDEFINED, w, h, flags) {
    
}

Sdl::Window::Window(const std::string& title, int x, int y,
                                    int w, int h, Uint32 flags)
    : window (SDL_CreateWindow(title.c_str(), x, y, w, h, flags)) {
    if (!window) {
        std::cerr << "Window could not be created! SDL_Error: " <<
                        SDL_GetError() << '\n';
        throw -1;
    }
}

Sdl::Window::~Window() {
    if (window) {
        SDL_DestroyWindow(window);
    }
}

SDL_Surface* Sdl::Window::getSurface() const {
    return SDL_GetWindowSurface(window);
}

SDL_PixelFormat* Sdl::Window::getFormat() const {
    return SDL_GetWindowSurface(window)->format;
}

SDL_Window* Sdl::Window::getWindow() const {
    return window;
}

void Sdl::Window::update() const {
    SDL_UpdateWindowSurface(window);
}
// Window //

// Image //
Sdl::Image::Image(const std::string& path_to_image, const SDL_PixelFormat* fmt)
    : Image(path_to_image) {
    auto optimized { SDL_ConvertSurface(image, fmt, 0) };
    if (!optimized) {
        std::cerr << "Unable to optimize image " << path_to_image <<
                        "! SDL_Error: " << SDL_GetError() << '\n';
        throw -1;
    }
    SDL_FreeSurface(image);
    image = optimized;
} 

Sdl::Image::Image(const std::string& path_to_image) 
    : image (IMG_Load(path_to_image.c_str())), name (path_to_image) {
    // : image(SDL_LoadBMP(path_to_image.c_str())) {
    if (!image) {
        std::cerr << "Unable to load image " << path_to_image <<
                        "! SDL_image_Error: " << IMG_GetError() << '\n';
        throw -1;
    }
}

Sdl::Image::~Image() {
    SDL_FreeSurface(image);
}

SDL_Surface* Sdl::Image::getSurface() const {
    return image;
}

std::string Sdl::Image::getName() const {
    return name;
}
// Image //

// Renderer //
Sdl::Renderer::Renderer(const Window& window, Uint32 flags)
    : renderer (SDL_CreateRenderer(window.getWindow(), -1, flags)) {
    if (!renderer) {
        std::cerr << "Renderer could not be created! SDL_Error: " <<
                        SDL_GetError() << '\n';
        throw -1;
    }
}

Sdl::Renderer::~Renderer() {
    SDL_DestroyRenderer(renderer);
}

SDL_Renderer* Sdl::Renderer::getRenderer() const {
    return renderer;
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

void Sdl::Renderer::copy(const Texture& texture, SDL_Rect src, SDL_Rect dst) const {
    SDL_RenderCopy(renderer, texture.getTexture(), &src, &dst);
}
// Renderer //

// Texture //
Sdl::Texture::Texture(const Renderer& renderer, const Image& image)
    : texture (SDL_CreateTextureFromSurface(renderer.getRenderer(),
                                            image.getSurface())) {
    if (!texture) {
        std::cerr << "Unable to create texture from " << image.getName() <<
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

// class Ball {
// public:
//     Ball(int x, int y, int r, int vx, int vy);
    
//     int x;
//     int y;
//     int r;
//     int vx;
//     int vy;
// private:
// };
// struct Ball {
//     int x;
//     int y;
//     int r;
//     double vx;
//     double vy;

//     void render(const Renderer& renderer, const Texture& tex) {
//         renderer.copy(tex, { x - r, y - r, 2 * r, 2 * r });
//     }

//     void update() {
//         x += vx;
//         y += vy;
//     }
// };

// TODO: add Viewable interface and RenderMan classes
// (if possible generate textures for viewables on the fly somehow)

class Ball {
public:
    Ball(const glm::dvec2& p, const glm::dvec2& v, double radius)
        : pos (p), vel (v), r (radius) {
        
    }
    glm::dvec2 pos;
    glm::dvec2 vel;
    double r;

    void render(const Sdl::Renderer& renderer, const Sdl::Texture& tex) {
        renderer.copy(tex, { static_cast<int>(pos.x - r), static_cast<int>(pos.y - r), static_cast<int>(2 * r), static_cast<int>(2 * r) });
    }
};

#include <cstdlib>
#include <ctime>
#include <functional>
#include <valarray>

template <typename T>
class Transform {
public:
    using transform = std::function<T(const T&)>;
    Transform(T& val, transform func, transform inv)
        : val_ (val), func_ (func), inv_ (inv) {
        transformed = func_(val_);
    }
    void operator=(const T& other) {
        transformed = other;
        val_ = inv_(transformed);
    }
    T& operator*() {
        return transformed;
    }
private:
    T& val_;
    T transformed;
    transform func_;
    transform inv_;
};

glm::dvec2 rotate2D(const glm::dvec2& vec, double ang);
glm::dvec2 rotate2D(const glm::dvec2& vec, double ang) {
    glm::dmat2 rotation_matrix { {std::cos(ang), -std::sin(ang)},
                                 {std::sin(ang),  std::cos(ang)} };
    return rotation_matrix * vec;
}

class PhysicsMan {
public:
    PhysicsMan(int w, int h, double cr)
        : width (w), height (h), coef (cr) {
        
    }
    void update(std::vector<Ball>& balls) {
        for (auto& b : balls) {
            b.pos += b.vel;
            if (b.pos.x - b.r < 0) {
                b.vel.x *= - coef;
                b.pos.x = b.r;
            } else if (b.pos.x + b.r > width) {
                b.vel.x *= - coef;
                b.pos.x = width - b.r;
            }
            if (b.pos.y - b.r < 0) {
                b.vel.y *= - coef;
                b.pos.y = b.r;
            } else if (b.pos.y + b.r > height) {
                b.vel.y *= - coef;
                b.pos.y = height - b.r;
            }
        }
        for (std::size_t i { 0 }; i < balls.size() - 1; ++i) {
            for (std::size_t j { i + 1 }; j < balls.size(); ++j) {
                Ball& a { balls[i] };
                Ball& b { balls[j] };
                if (glm::distance(a.pos, b.pos) <= a.r + b.r) {
                    double angle { std::atan2(a.pos.y - b.pos.y, a.pos.x - b.pos.x) };
                    std::cout << "dy: " << a.pos.y - b.pos.y << "; dx: " << a.pos.x - b.pos.x << "; angulo: " << angle * 180.0 / 3.1415926535 << '\n';
                    auto   rot { std::bind(rotate2D, std::placeholders::_1,  angle) };
                    auto unrot { std::bind(rotate2D, std::placeholders::_1, -angle) };

                    Transform<glm::dvec2> vel_a_ { a.vel, rot, unrot };
                    Transform<glm::dvec2> vel_b_ { b.vel, rot, unrot };
                    Transform<glm::dvec2> pos_a_ { a.pos, rot, unrot };
                    Transform<glm::dvec2> pos_b_ { b.pos, rot, unrot };
                    double mass_a { 3.14 * a.r * a.r };
                    double mass_b { 3.14 * b.r * b.r };
                    glm::dvec2 vcm { (mass_a * (*vel_a_) + mass_b * (*vel_b_)) / (mass_a + mass_b) };
                    
                    vel_a_ = (1.0 + coef) * vcm - coef * (*vel_a_);
                    vel_b_ = (1.0 + coef) * vcm - coef * (*vel_b_);
                    
                    double overlap { a.r + b.r - ((*pos_a_).x - (*pos_b_).x) };
                    pos_a_ = (*pos_a_) + glm::dvec2 { overlap / 2.0, 0.0 };
                    pos_b_ = (*pos_b_) - glm::dvec2 { overlap / 2.0, 0.0 };
                }
            }
        }
    }
private:
    double width;
    double height;
    double coef;
};


int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[]) {
    constexpr int screen_width { 640 };
    constexpr int screen_height { 480 };
    std::srand(std::time(nullptr));
    // SDL_Surface* screenSurface { nullptr };

    Sdl interface { SDL_INIT_VIDEO };
    interface.setAntiAliasing("1");
    Sdl::Window& window { interface.createWindow("Collision simulation", screen_width, screen_height) };

    // screenSurface = window.getSurface();
    // Image image { "texture.png", window.getFormat() };
    Sdl::Image image { "texture.png" };

    std::cout << "kk\n";
    Sdl::Renderer renderer { window, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE };
    Sdl::Texture texture { renderer, image };

    Sdl::Image circle { "circle.png" };
    Sdl::Texture tex { renderer, circle };

    SDL_Event e;
    bool quit { false };
    // int n_balls { std::rand() % 17 + 3 };
    int n_balls { 3 };
    std::vector<Ball> balls;
    for (int i { 0 }; i < n_balls; ++i) {
        // double radius { std::rand() % 10 + 10.0 };
        double radius { 40.0 };
        double x { std::rand() % (screen_width - 2 * static_cast<int>(radius)) + radius };
        double y { std::rand() % (screen_height - 2 * static_cast<int>(radius)) + radius };
        // balls.push_back({{x, y}, {std::rand() % 8 + 2.0, std::rand() % 8 + 2.0}, radius});
        balls.push_back({{x, y}, {(std::rand() % 8 + 2.0) / 100.0, (std::rand() % 8 + 2.0) / 100.0}, radius});
    }
    balls.push_back({{300.0, 150.0}, {0.0, 0.0}, 5.0});
    PhysicsMan guy { screen_width, screen_height, 1.0 };
    while (!quit) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
        }
        renderer.setDrawColor(0xff, 0xff, 0xff, 0xff);
        renderer.clear();
        renderer.setDrawColor(0x00, 0x00, 0x00, 0xff);
        guy.update(balls);

        for (auto& b : balls) {
            b.render(renderer, tex);
        }
        renderer.swap();
    }
    return 0;
}