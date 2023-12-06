#ifndef LINE2F
#define LINE2F

#include "Pos2F.h"
#include "Vec2F.h"

class Line2F {
    public:
    Line2F(const Pos2F& one, const Pos2F& two) : a(one), b(two) {}
    Line2F(float ax, float ay, float bx, float by) : a(ax,ay), b(bx,by) {}

    Vec2F GetForwardVec() const {
        return b.Subtract(a);
    }
    Vec2F GetBackwardVec() const {
        return a.Subtract(b);
    }

    Pos2F GetP1() const { return a;}
    Pos2F GetP2() const { return b;}
    void SetP1Y(float y) { a.y = y; }
    void SetP2Y(float y) { b.y = y; }
    private:
        Pos2F a;
        Pos2F b;
};

#endif