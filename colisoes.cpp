#include <iostream>
#include <sstream>
#include <vector>
#include <random>       // Para std::mt1997 e std::uniform_real_distribution
#include <chrono>       // Para gerar uma semente para o PRNG acima citado

#include "sdl_interface.hpp"    // Onde está a bagunça
#include "physics.hpp"          // Onde está a bolinha

// Função usada para receber entrada junto com a invocação do programa
double toDouble(const char* str);

class Randomizer {
public:
    Randomizer()
        : mersenne (std::chrono::steady_clock::now()
                    .time_since_epoch()
                    .count()) { }
    double rand(double min, double max) {
        return std::uniform_real_distribution<double> {min, max}(mersenne);
    }
private:
    std::mt19937 mersenne;
};

int main(int argc, char* argv[]) {
    if (argc < 4 || argc > 6) {
        std::cout << "Uso: colisoes(.exe) largura altura n_bolas " <<
                     "[raio] [coef_restituicao=1.0]\n";
        std::cout << "Se o raio das bolas nao for especificado, cada bola " <<
                     "tera um raio aleatorio\n";
        return -1;
    }
    
    int screen_width { static_cast<int>(toDouble(argv[1])) };
    int screen_height { static_cast<int>(toDouble(argv[2])) };
    int n_balls { static_cast<int>(toDouble(argv[3])) };
    double rad { 0 };
    if (argc >= 5) {
        rad = toDouble(argv[4]);
    }
    double coef { 1.0 };
    if (argc == 6) {
        coef = toDouble(argv[5]);
    }

    Randomizer gen;

    Sdl interface { SDL_INIT_VIDEO };
    interface.setAntiAliasing("1");
    Sdl::Window& window { interface.createWindow("Collision simulation",
                          screen_width, screen_height) };
    Sdl::Renderer& renderer { window.getRenderer() };
    Sdl::Texture circle { renderer, "circle.png" };
    
    std::vector<Ball> balls;
    for (int i { 0 }; i < n_balls; ++i) {
        double radius { rad ? rad : gen.rand(20.0, 60.0) };
        
        double x { gen.rand(radius, screen_width  - radius) };
        double y { gen.rand(radius, screen_height - radius) };
        double vx { gen.rand(-0.2, 0.2) };
        double vy { gen.rand(-0.2, 0.2) };
        
        balls.push_back({{x, y}, { vx, vy }, radius, 1.0});
    }
    
    PhysicsMan guy { screen_width, screen_height, coef };
    bool running { true };
    interface.setOnSpaceDownCallback([&] () { running = !running; });
    while (interface.openWindows()) {
        interface.handleEvents();

        renderer.setDrawColor(0x00, 0xbf, 0xbf, 0xff);
        renderer.clear();
        renderer.setDrawColor(0x00, 0x00, 0x00, 0xff);

        if (running) {
            guy.update(balls);
        }

        for (auto& b : balls) {
            renderer.copy(circle, { static_cast<int>(b.pos.x - b.r),
                                    static_cast<int>(b.pos.y - b.r),
                                    static_cast<int>(2 * b.r),
                                    static_cast<int>(2 * b.r) } );
        }
        renderer.swap();
    }
    return 0;
}

double toDouble(const char* str) {
    std::stringstream ss;
    ss << str;
    double ret { 0 };
    ss >> ret;
    return ret;
}