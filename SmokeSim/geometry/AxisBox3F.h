#ifndef AXISBOX3F
#define AXISBOX3F

#include "Pos3F.h"
#include "Vec3F.h"

class AxisBox3F {
    public:
        // p is has maximum values of X, Y, Z
        // n has minimum values of X, Y, Z
        AxisBox3F(const Pos3F& p, const Pos3F& n) :
            pos(p), neg(n) {

            }

        Pos3F GetPos() const {return pos;}
        Pos3F GetNeg() const {return neg;}
    private:
        Pos3F pos, neg;
        
};

#endif