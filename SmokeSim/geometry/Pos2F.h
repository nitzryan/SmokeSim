#ifndef POS2F
#define POS2F

#include "Vec2F.h"

struct Pos2F {
    float x;
    float y;

    Pos2F(float xx, float yy) {
        x = xx;
        y = yy;
    }

    Pos2F operator=(const Pos2F& p) {
        x = p.x;
        y = p.y;
        return *this;
    }

    static Pos2F Add(const Pos2F& p, const Vec2F& v) {
        return Pos2F(p.x + v.x, p.y + v.y);
    }

    void Add(const Vec2F& v) {
        x += v.x;
        y += v.y;
    }

    Vec2F Subtract(const Pos2F& p) const {
        return Vec2F(x - p.x, y - p.y);
    }

    static Vec2F SubtractBFromA(const Pos2F& a, const Pos2F& b) {
        return Vec2F(a.x - b.x, a.y - b.y);
    }

    Vec2F operator-(const Pos2F& p) const {
        return Subtract(p);
    }

    Pos2F operator-(const Vec2F& v) const {
        return Pos2F(x - v.x, y - v.y);
    }

    Pos2F operator+(const Vec2F& v) const {
        return Pos2F::Add(*this, v);
    }
};

#endif