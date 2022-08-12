#pragma once

#include <math.h>
#include <Windows.h>

double distance(const POINT& p1, const POINT& p2)
{
    double dx = static_cast<double>(p1.x) - static_cast<double>(p2.x);
    double dy = static_cast<double>(p1.y) - static_cast<double>(p2.y);
    return sqrt(pow(dx, 2) + pow(dy, 2));
}