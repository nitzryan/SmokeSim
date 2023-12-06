#ifndef LINE3F
#define LINE3F

#include "Pos3F.h"
#include "Vec3F.h"

class Line3F {
    public:
    Line3F(const Pos3F& one, const Pos3F& two) : a(one), b(two) {}

    Vec3F GetForwardVec() const {
        return b.Subtract(a);
    }
    Vec3F GetBackwardVec() const {
        return a.Subtract(b);
    }

    Pos3F GetP1() const { return a;}
    Pos3F GetP2() const { return b;}
    private:
        Pos3F a;
        Pos3F b;
};

#endif