#ifndef VEC2F
#define VEC2F

#include <math.h>

struct Vec2F {
    float x;
    float y;

    Vec2F() {
        x = 0;
        y = 0;
    }

    Vec2F(float xx, float yy) {
        x = xx;
        y = yy;
    };

    static Vec2F Add(const Vec2F& v1, const Vec2F& v2) {
        return Vec2F(v1.x + v2.x, v1.y + v2.y);
    }
    void Add(const Vec2F& v) {
        x += v.x;
        y += v.y;
    }

    Vec2F operator+(const Vec2F& v) const {
        return Vec2F::Add(*this, v);
    }

    static Vec2F Sub(const Vec2F& v1, const Vec2F& v2) {
        return Vec2F(v1.x - v2.x, v1.y - v2.y);
    }
    void Sub(const Vec2F& v) {
        x -= v.x;
        y -= v.y;
    }

    Vec2F operator-(const Vec2F& v) const {
        return Vec2F::Sub(*this, v);
    }

    static Vec2F Mul(const Vec2F& v, float m) {
        return Vec2F(v.x * m, v.y * m);
    }
    void Mul(float m) {
        x *= m;
        y *= m;
    }

    float GetMagnitudeSquared() const {
        return Vec2F::Dot(*this, *this);
    }
    float GetMagnitude() const {
        return sqrtf(GetMagnitudeSquared());
    }
    Vec2F GetNormalized() const {
        float scale = 1 / GetMagnitude();
        return Vec2F::Mul(*this, scale);
    }
    void Normalize() {
        float scale = 1 / GetMagnitude();
        x *= scale;
        y *= scale;
    }

    static float Dot(const Vec2F& v1, const Vec2F& v2) {
        return v1.x * v2.x + v1.y * v2.y;
    } 

    static float Cross(const Vec2F& v1, const Vec2F& v2) {
        return v1.x * v2.y - v1.y * v2.x;
    }

    static Vec2F Proj(const Vec2F& vec, const Vec2F& dir) {
        auto dirNorm = dir.GetNormalized();
        dirNorm.Mul(Vec2F::Dot(dirNorm, vec));
        return dirNorm;
    }

    static Vec2F GetPerp(const Vec2F& vec) {
        return Vec2F(vec.y, -vec.x);
    }
};

#endif