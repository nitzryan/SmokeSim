#ifndef SPHERE
#define SPHERE

#include "Pos3F.h"

class Circle {
    public:
        Circle(float r, const Pos3F& p) : pos(p) {
            radius = r;
        }

        float GetCircumference() const {
            return (2 * 3.141592653589) * radius;
        }

        float GetRadius() const {return radius;}
        Pos3F GetCenter() const {return pos;}
        void SetPos(const Pos3F& p) { pos.x = p.x; pos.y = p.y; }
    private:
        float radius;
        Pos3F pos;  
};

#endif