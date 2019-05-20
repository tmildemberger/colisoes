#ifndef PHYSICS_HPP
#define PHYSICS_HPP

#include <functional>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"
#pragma GCC diagnostic ignored "-Wpedantic"
#include <glm/glm.hpp>
#pragma GCC diagnostic pop

glm::dvec2 rotate2D(const glm::dvec2& vec, double ang);

class Ball {
public:
    Ball(const glm::dvec2& p, const glm::dvec2& v, double radius,
                                                   double area_density);
    glm::dvec2 pos;
    glm::dvec2 vel;
    double r;
    double mass;
};

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

// Esse é o cara que entende de física
class PhysicsMan {
public:
    PhysicsMan(int w, int h, double cr);
    void update(std::vector<Ball>& balls);
private:
    double width;
    double height;
    double coef;
};

#endif // PHYSICS_HPP