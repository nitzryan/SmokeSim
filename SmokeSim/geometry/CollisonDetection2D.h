#pragma once

#include "Pos2F.h"
#include "Vec2F.h"
//#include "AxisBox2F.h"
#include "Line2F.h"
//#include "Circle.h"
#include <assert.h>

float clamp(float min, float max, float val);
bool SameSide(const Line2F& l, const Pos2F& p1, const Pos2F& p2);
bool CollisionBetween(const Line2F& l1, const Line2F& l2, Pos2F& colPoint);
//bool CollisionBetween(const Line2F& l1, const Circle& c2);
//bool CollisionBetweenPos(const Line2F& l, const Circle& c, Pos2F& tPos, Pos2F& circleCenterPos, Vec2F& colAxis);
//bool CollisionBetween(const Line2F& l1, const AxisBox2F& ab2);
//bool CollisionBetween(const Circle& c1, const Circle& c2);
//bool CollisionBetweenPos(const Circle& c1, const Circle& c2, Pos2F& colPoint, Pos2F& newC1Center, Pos2F& newC2Center, float c1MassFrac);
//bool CollisionBetween(const Circle& c1, const AxisBox2F& ab2);
//bool CollisionBetween(const AxisBox2F& ab1, const AxisBox2F& ab2);