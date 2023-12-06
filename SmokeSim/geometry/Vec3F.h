#ifndef VEC3F
#define VEC3F

#include <math.h>

struct Vec3F {
    float x;
    float y;
    float z;

    Vec3F() {
        x = 0;
        y = 0;
        z = 0;
    }

    Vec3F(float xx, float yy, float zz) {
        x = xx;
        y = yy;
        z = zz;
    };

    static Vec3F Add(const Vec3F& v1, const Vec3F& v2) {
        Vec3F v = v1;
        v.Add(v2);
        return v;
    }
    void Add(const Vec3F& v) {
        x += v.x;
        y += v.y;
        z += v.z;
    }

    Vec3F operator+(const Vec3F& v) const {
        return Vec3F::Add(*this, v);
    }

    Vec3F operator+=(const Vec3F& v) {
        Add(v);
        return *this;
    }

    static Vec3F Sub(const Vec3F& v1, const Vec3F& v2) {
        Vec3F v = v1;
        v.Sub(v2);
        return v;
    }
    void Sub(const Vec3F& v) {
        x -= v.x;
        y -= v.y;
        z -= v.z;
    }

    static Vec3F Mul(const Vec3F& v, float m) {
        Vec3F res = v;
        res.Mul(m);
        return res;
    }
    void Mul(float m) {
        x *= m;
        y *= m;
        z *= m;
    }

    float GetMagnitudeSquared() const {
        return Vec3F::Dot(*this, *this);
    }
    float GetMagnitude() const {
        return sqrtf(GetMagnitudeSquared());
    }
    Vec3F GetNormalized() const {
        float scale = 1 / GetMagnitude();
        return Vec3F::Mul(*this, scale);
    }
    void Normalize() {
        float scale = 1 / GetMagnitude();
        this->Mul(scale);
    }

    static float Dot(const Vec3F& v1, const Vec3F& v2) {
        return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
    } 

    static Vec3F Cross(const Vec3F& v1, const Vec3F& v2) {
        float cx = v1.y * v2.z - v1.z * v2.y;
        float cy = v1.z * v2.x - v1.x * v2.z;
        float cz = v1.x * v2.y - v1.y * v2.x;
        return Vec3F(cx, cy, cz);
    }

    static Vec3F Proj(const Vec3F& vec, const Vec3F& dir) {
        auto dirNorm = dir.GetNormalized();
        dirNorm.Mul(Vec3F::Dot(dirNorm, vec));
        return dirNorm;
    }
};

#endif