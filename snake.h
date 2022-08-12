#pragma once

#include <commctrl.h>
#include <vector>
#include <windows.h>

class Snake
{
public:
    Snake(HWND hndl, HANDLE handle);

    void move();
    void checkCollision();
    void setHeadDirection(Action action);
    void explode(); // FIXME

private:
    HWND m_hndl;
    HANDLE m_handle;
    Direction m_headDirection = Direction::LEFT;
    std::vector<int> m_indexes = { 0 };
    std::vector<POINT> m_position = { {600, 50}, {610, 50}, {620, 50}, {630, 50}, {640, 50}, {650, 50}, {660, 50} };
    RECT m_desktopResolution;
    std::vector<Icon> m_iconsPositions;
    size_t m_counter;
    const uint8_t m_sensativityLimit;

    void initHeadPosition();
    void moveBody();
    void moveHead();
    void teleportIfNecessary(POINT& point);
    void initIconsPositions();
};