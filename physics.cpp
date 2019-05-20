#include "physics.hpp"
#include <cmath>

glm::dvec2 rotate2D(const glm::dvec2& vec, double ang) {
    glm::dmat2 rotation_matrix { {std::cos(ang), -std::sin(ang)},
                                 {std::sin(ang),  std::cos(ang)} };
    return rotation_matrix * vec;
}

// Ball //
Ball::Ball(const glm::dvec2& p, const glm::dvec2& v, double radius,
                                                     double area_density)
    : pos (p), vel (v), r (radius),
                        mass (area_density * r * r * std::atan(1) * 4.0) {
    
}
// Ball //

// PhysicsMan //
PhysicsMan::PhysicsMan(int w, int h, double cr)
    : width (w), height (h), coef (cr) {
    
}

void PhysicsMan::update(std::vector<Ball>& balls) {
    for (auto& b : balls) {
        b.pos += b.vel;
        // Colisões com as paredes
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
                double angle {std::atan2(a.pos.y - b.pos.y, a.pos.x - b.pos.x)};
                
                auto   rot {std::bind(rotate2D, std::placeholders::_1,  angle)};
                auto unrot {std::bind(rotate2D, std::placeholders::_1, -angle)};

                // Essa classe transforma os vetores de um sistema de
                // coordenadas para outra, de forma que quando alteramos
                // o vetor na forma transformada, o vetor original
                // também é alterado se desfazendo a transformação
                Transform<glm::dvec2> vel_a_ { a.vel, rot, unrot };
                Transform<glm::dvec2> vel_b_ { b.vel, rot, unrot };
                Transform<glm::dvec2> pos_a_ { a.pos, rot, unrot };
                Transform<glm::dvec2> pos_b_ { b.pos, rot, unrot };
                // Não funciona ao contrário, e não funciona mudar
                // componentes do vetor
                
                glm::dvec2 vcm { (a.mass * (*vel_a_) + b.mass * (*vel_b_)) /
                                 (a.mass + b.mass) };
                
                vel_a_ = {((1.0 + coef) * vcm - coef * (*vel_a_)).x,
                          (*vel_a_).y};
                vel_b_ = {((1.0 + coef) * vcm - coef * (*vel_b_)).x,
                          (*vel_b_).y};
                
                // "Hack" para separar as bolas, pretendo arrumar depois
                double overlap { a.r + b.r - ((*pos_a_).x - (*pos_b_).x) };
                pos_a_ = (*pos_a_) + glm::dvec2 { overlap * b.mass /
                                                  (a.mass + b.mass), 0.0 };
                pos_b_ = (*pos_b_) - glm::dvec2 { overlap * a.mass /
                                                  (a.mass + b.mass), 0.0 };
            }
        }
    }
}
// PhysicsMan //