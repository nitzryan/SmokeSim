#include "CollisonDetection2D.h"

const float SEPARATION_WIDTH = 0.005f;

float clamp(float min, float max, float val) {
    if (val < min)
        return min;
    if (val > max)
        return max;
    return val;
}

bool SameSide(const Line2F& l, const Pos2F& p1, const Pos2F& p2) {
    auto v = l.GetForwardVec();
    auto cross1 = Vec2F::Cross(v, p1.Subtract(l.GetP1()));
    auto cross2 = Vec2F::Cross(v, p2.Subtract(l.GetP1()));
    return cross1 * cross2 >= 0; // Check for same sign
};

bool CollisionBetween(const Line2F& l1, const Line2F& l2, Pos2F& colPoint) {
    bool hasCol = !(SameSide(l1, l2.GetP1(), l2.GetP2()) || SameSide(l2, l1.GetP1(), l1.GetP2()));
    if (!hasCol) {
        return false;
    }

    // https://en.wikipedia.org/wiki/Line%E2%80%93line_intersection
    auto p1 = l1.GetP1();
    auto p2 = l1.GetP2();
    auto p3 = l2.GetP1();
    auto p4 = l2.GetP2();
    float x3mx4 = p3.x - p4.x;
    float x1mx2 = p1.x - p2.x;
    float y3my4 = p3.y - p4.y;
    float y1my2 = p1.y - p2.y;
    float det = x1mx2 * y3my4 - y1my2 * x3mx4;
    float x = ((p1.x * p2.y - p1.y * p2.x) * x3mx4 - (p3.x * p4.y - p3.y * p4.x) * x1mx2) / det;
    float y = ((p1.x * p2.y - p1.y * p2.x) * y3my4 - (p3.x * p4.y - p3.y * p4.x) * y1my2) / det;
    colPoint = Pos2F(x, y);

    return true;
}

//bool CollisionBetween(const Line2F& l1, const Circle& c2) {
//    // Cast Ray from l1.p1 to l1.v2
//    // Check if Ray intersects c2
//    auto vec = l1.GetForwardVec();
//    auto lineLen = vec.GetMagnitude();
//    vec.Normalize();
//
//    auto pos = l1.GetP1();
//    auto radius = c2.GetRadius();
//    auto center = c2.GetCenter();
//
//    // Check if pos is inside of the circle
//    if (pos.Subtract(center).GetMagnitude() < radius)
//        return true;
//
//    // Solve Quadratic formula
//    auto P0minusC = pos.Subtract(center);
//    auto b = 2 * Vec2F::Dot(vec, P0minusC);
//    auto c = P0minusC.GetMagnitudeSquared() - radius * radius; \
//
//        auto det = b * b - 4 * c;
//    if (det < 0)
//        return false; // No intersection point
//
//    det = sqrt(det);
//    auto t1 = 0.5 * (det - b);
//    auto t2 = 0.5 * (-det - b);
//
//    // Check if either collision point is between the start point and the end point
//    return (t1 > 0 && t1 < lineLen) || (t2 > 0 && t2 < lineLen);
//}
//
//bool CollisionBetweenPos(const Line2F& l, const Circle& c, Pos2F& tPos, Pos2F& circleCenterPos, Vec2F& colAxis) {
//    if (!CollisionBetween(l, c))
//        return false;
//
//    // Collision will occur with center r distance above line
//    // project circle coords, circle vec into coordinate system of line
//    Vec2F lineYAxis = Vec2F::GetPerp(l.GetBackwardVec());
//    lineYAxis.Normalize();
//
//    // Get y coordinate of line
//    Pos2F p0 = l.GetP1();
//    float y0 = p0.x * lineYAxis.x + p0.y * lineYAxis.y;
//    float yTarget = y0 + c.GetRadius();
//
//    // Get y coordinate of circle center
//    Pos2F pc = c.GetCenter();
//    float yc = pc.x * lineYAxis.x + pc.y * lineYAxis.y;
//    float yDelta = yc - yTarget;
//
//    // Check that the collision point is actually along the line
//    // Line may have been collided with through the side, which means the circle
//    // Hit another object first, so this tCol will be wrong
//    Vec2F lineXAxis = l.GetForwardVec();
//    float magnitude = lineXAxis.GetMagnitude();
//    lineXAxis.Normalize();
//    float x0 = p0.x * lineXAxis.x + p0.y * lineXAxis.y;
//    float xc = pc.x * lineXAxis.x + pc.y * lineXAxis.y;
//
//    tPos = Pos2F::Add(p0, Vec2F::Mul(lineXAxis, xc - x0));
//    circleCenterPos = Pos2F::Add(tPos, Vec2F::Mul(lineYAxis, c.GetRadius() + SEPARATION_WIDTH));
//    colAxis = lineYAxis;
//
//    return true;
//}
//
//bool CollisionBetween(const Line2F& l1, const AxisBox2F& ab2) {
//    auto ll = ab2.GetLL();
//    auto ur = ab2.GetUR();
//
//    auto p1 = l1.GetP1();
//
//    // To check collision, take first point.  Only check left
//    // side if x is less than left side.  Continue for all points
//    // If no conditions are satisfied, it means the point is inside
//    // the box, so it is 
//    bool noneChecked = true;
//    if (p1.x < ll.x) { // Left
//        noneChecked = false;
//        if (CollisionBetween(l1, Line2F(ll.x, ll.y, ll.x, ur.y)))
//            return true;
//    }
//    else if (p1.x > ur.x) { // Right
//        noneChecked = false;
//        if (CollisionBetween(l1, Line2F(ur.x, ll.y, ur.x, ur.y)))
//            return true;
//    }
//    if (p1.y < ll.y) { /// Bot
//        noneChecked = false;
//        if (CollisionBetween(l1, Line2F(ll.x, ll.y, ur.x, ll.y)))
//            return true;
//    }
//    else if (p1.y > ur.y) { // Top
//        noneChecked = false;
//        if (CollisionBetween(l1, Line2F(ll.x, ur.y, ur.x, ur.y)))
//            return true;
//    }
//
//    return noneChecked;
//}
//
//bool CollisionBetween(const Circle& c1, const Circle& c2) {
//    auto center1 = c1.GetCenter();
//    auto center2 = c2.GetCenter();
//
//    return center1.Subtract(center2).GetMagnitude() < (c1.GetRadius() + c2.GetRadius());
//}
//
//bool CollisionBetweenPos(const Circle& c1, const Circle& c2, Pos2F& colPoint, Pos2F& newC1Center, Pos2F& newC2Center, float c1MassFrac) {
//    auto center1 = c1.GetCenter();
//    auto center2 = c2.GetCenter();
//
//    auto dist = center1.Subtract(center2);
//    float mag = dist.GetMagnitude();
//    float rads = c1.GetRadius() + c2.GetRadius();
//    if (mag >= rads)
//        return false;
//
//    colPoint = Pos2F::Add(center2, Vec2F::Mul(dist, 0.5));
//    dist.Normalize();
//    dist.Mul(rads - mag);
//    newC2Center = Pos2F::Add(center2, Vec2F::Mul(dist, -c1MassFrac - SEPARATION_WIDTH / 2));
//    newC1Center = Pos2F::Add(center1, Vec2F::Mul(dist, (1 - c1MassFrac) + SEPARATION_WIDTH / 2));
//
//    return true;
//}
//
//bool CollisionBetween(const Circle& c1, const AxisBox2F& ab2) {
//    auto ll = ab2.GetLL();
//    auto ur = ab2.GetUR();
//
//    auto center = c1.GetCenter();
//
//    auto closestPoint = Pos2F(
//        clamp(ll.x, ur.x, center.x),
//        clamp(ll.y, ur.y, center.y)
//    );
//
//    auto dist = center.Subtract(closestPoint).GetMagnitude();
//    return dist < c1.GetRadius();
//}
//
//bool CollisionBetween(const AxisBox2F& ab1, const AxisBox2F& ab2) {
//    auto ll1 = ab1.GetLL();
//    auto ur1 = ab1.GetUR();
//    auto ll2 = ab2.GetLL();
//    auto ur2 = ab2.GetUR();
//
//    return ll1.x < ur2.x&&
//        ll2.x < ur1.x&&
//        ll1.y < ur2.y&&
//        ll2.y < ur1.y;
//}
