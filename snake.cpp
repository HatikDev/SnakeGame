#include "constants.h"
#include "snake.h"
#include "util.h"

#include <commctrl.h>
#include <vector>

Snake::Snake(HWND hndl, HANDLE handle) : m_hndl{ hndl }, m_handle{ handle }, m_sensativityLimit{ 1440 / SENSITIVITY_COEFF } // FIXME
{
    GetWindowRect(GetDesktopWindow(), &m_desktopResolution);

    initIconsPositions();
    initHeadPosition();
}

void Snake::initHeadPosition()
{
    POINT pos = { 600, 50 };
    ListView_SetItemPosition(m_hndl, 0, pos.x, pos.y);
}

void Snake::move()
{
    moveHead();
    moveBody();
}

void Snake::checkCollision()
{
    for (size_t i = 0; i < m_iconsPositions.size(); ++i)
    {
        if (distance(m_iconsPositions[i].position, m_position[0]) < m_sensativityLimit)
        {
            ListView_SetItemPosition(m_hndl, m_iconsPositions[i].index, m_position[m_position.size() - 1].x, m_position[m_position.size() - 1].y);

            for (size_t j = 0; j < 7; ++j)
                m_position.push_back(m_position[m_position.size() - 1]);
            m_indexes.push_back(m_iconsPositions[i].index);
            m_iconsPositions.erase(m_iconsPositions.begin() + i);
            m_counter = 7;
            break;
        }
    }
}

void Snake::setHeadDirection(Action action)
{
    if (action == Action::TURNLEFT && m_headDirection != Direction::RIGHT)
        m_headDirection = Direction::LEFT;

    if (action == Action::TURNUP && m_headDirection != Direction::DOWN)
        m_headDirection = Direction::UP;

    if (action == Action::TURNRIGHT && m_headDirection != Direction::LEFT)
        m_headDirection = Direction::RIGHT;

    if (action == Action::TURNDOWN && m_headDirection != Direction::UP)
        m_headDirection = Direction::DOWN;
}

void Snake::explode()
{
    srand(NULL);

    std::vector<std::pair<double, double>> speeds;
    for (auto index : m_indexes)
        speeds.push_back({ rand() % EXPLOSION_MAX_SPEED, rand() % 6 });

    DWORD time = EXPLOSION_DELAY_INTERVAL;
    for (size_t i = 0; i < EXPLOTION_ITERATION_COUNT; ++i)
    {
        for (size_t j = 0; j < m_indexes.size(); ++j)
        {
            auto [speed, angle] = speeds[j];
            m_position[7 * j].x += static_cast<LONG>(speed * cos(angle));
            m_position[7 * j].y += static_cast<LONG>(speed * sin(angle));
            teleportIfNecessary(m_position[7 * j]);
            ListView_SetItemPosition(m_hndl, m_indexes[j], m_position[7 * j].x, m_position[7 * j].y);
        }

        Sleep(time);
        time += EXPLOSION_SLOWDOWN;
    }
}

void Snake::moveBody()
{
    for (size_t i = m_position.size() - 1 - m_counter; i > 0; --i)
        m_position[i] = m_position[i - 1];

    for (size_t i = 1; i < m_indexes.size(); ++i)
        ListView_SetItemPosition(m_hndl, m_indexes[i], m_position[7 * i].x, m_position[7 * i].y);

    if (m_counter)
        --m_counter;
}

void Snake::moveHead()
{
    switch (m_headDirection)
    {
    case Direction::UP:
        m_position[0].y -= m_desktopResolution.bottom / HEIGHT_SPEED_COEFF;
        break;
    case Direction::RIGHT:
        m_position[0].x += m_desktopResolution.right / WIDTH_SPEED_COEFF;
        break;
    case Direction::DOWN:
        m_position[0].y += m_desktopResolution.bottom / HEIGHT_SPEED_COEFF;
        break;
    case Direction::LEFT:
        m_position[0].x -= m_desktopResolution.right / WIDTH_SPEED_COEFF;
        break;
    }

    teleportIfNecessary(m_position[0]);

    ListView_SetItemPosition(m_hndl, 0, m_position[0].x, m_position[0].y);
}

void Snake::teleportIfNecessary(POINT& point)
{
    if (point.x < 0)
        point.x = m_desktopResolution.right;
    if (point.x > m_desktopResolution.right)
        point.x = 0;
    if (point.y < 0)
        point.y = m_desktopResolution.bottom;
    if (point.y > m_desktopResolution.bottom)
        point.y = 0;
}

void Snake::initIconsPositions()
{
    POINT* pC = (POINT*)VirtualAllocEx(m_handle, NULL, sizeof(POINT), MEM_COMMIT, PAGE_READWRITE);
    POINT pt;

    int count = static_cast<int>(SendMessage(m_hndl, LVM_GETITEMCOUNT, 0, 0));
    for (uint8_t i = 1; i < count; ++i)
    {
        WriteProcessMemory(m_handle, pC, &pC, sizeof(POINT), NULL);

        ListView_GetItemPosition(m_hndl, i, pC);
        ReadProcessMemory(m_handle, pC, &pt, sizeof(POINT), NULL);

        m_iconsPositions.push_back({ i, pt });
    }

    VirtualFreeEx(m_handle, pC, 0, MEM_RELEASE);
}