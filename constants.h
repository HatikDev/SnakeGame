#pragma once

#include <cstdint>
#include <windows.h>

#define KEY_PRESSED(key) GetKeyState(key)&0x8000

enum class Direction : uint8_t
{
    UP = 0,
    RIGHT,
    DOWN,
    LEFT,
};
enum class Action : uint8_t
{
    CONTINUE = 0,
    STOP,
    TURNLEFT,
    TURNUP,
    TURNDOWN,
    TURNRIGHT,
    EXPLODE,
    BOOST,
    PAUSE,
    RESTORE,
};
struct Icon
{
    uint8_t index;
    POINT position;
};

const uint8_t WIDTH_SPEED_COEFF = 115;
const uint8_t HEIGHT_SPEED_COEFF = 86;
const uint8_t SENSITIVITY_COEFF = 96;

const uint8_t EXPLOTION_ITERATION_COUNT = 8; // FIXME
const uint8_t EXPLOSION_DELAY_INTERVAL = 20;
const uint8_t EXPLOSION_MAX_SPEED = 40;      // FIXME
const uint8_t EXPLOSION_SLOWDOWN = 20;       // FIXME