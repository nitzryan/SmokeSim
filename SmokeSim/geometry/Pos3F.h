#ifndef POS3F
#define POS3F

#include "Vec3F.h"

struct Pos3F {
    float x;
    float y;
    float z;

    Pos3F() {
        x = 0;
        y = 0;
        z = 0;
    }

    Pos3F(float xx, float yy, float zz) {
        x = xx;
        y = yy;
        z = zz;
    }

    Pos3F operator=(const Pos3F& p) {
        x = p.x;
        y = p.y;
        z = p.z;
        return *this;
    }

    static Pos3F Add(const Pos3F& p, const Vec3F& v) {
        Pos3F res = p;
        res.Add(v);
        return res;
    }

    void Add(const Vec3F& v) {
        x += v.x;
        y += v.y;
        z += v.z;
    }

    void Sub(const Vec3F& v) {
		x -= v.x;
		y -= v.y;
		z -= v.z;
	}

    Vec3F Subtract(const Pos3F& p) const {
        return Vec3F(x - p.x, y - p.y, z - p.z);
    }

    Vec3F operator-(const Pos3F& p) const {
        return Subtract(p);
    }
};

#endif