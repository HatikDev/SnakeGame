#include "constants.h"
#include "snake.h"

#include <commctrl.h>
#include <vector>
#include <windows.h>

std::vector<Icon> initialIconsState;

std::vector<Icon> saveState(HWND hndl, HANDLE handle)
{
    POINT* pC = (POINT*)VirtualAllocEx(handle, NULL, sizeof(POINT), MEM_COMMIT, PAGE_READWRITE);
    POINT pt;

    int count = static_cast<int>(SendMessage(hndl, LVM_GETITEMCOUNT, 0, 0));
    std::vector<Icon> tempIcons;

    for (uint8_t i = 0; i < count; ++i)
    {
        WriteProcessMemory(handle, pC, &pC, sizeof(POINT), NULL);

        ListView_GetItemPosition(hndl, i, pC);

        ReadProcessMemory(handle, pC, &pt, sizeof(POINT), NULL);

        tempIcons.push_back({ i, pt });
    }

    VirtualFreeEx(handle, pC, 0, MEM_RELEASE);

    return tempIcons;
}

void resoreAllIconsPosition(HWND handle)
{
    for (auto icon : initialIconsState)
    {
        ListView_SetItemPosition(handle, icon.index, icon.position.x, icon.position.y);
    }
}

Action processKeyBoard(Snake& snake)
{
    if (KEY_PRESSED(VK_UP))
        return Action::TURNUP;

    if (KEY_PRESSED(VK_DOWN))
        return Action::TURNDOWN;

    if (KEY_PRESSED(VK_LEFT))
        return Action::TURNLEFT;

    if (KEY_PRESSED(VK_RIGHT))
        return Action::TURNRIGHT;

    if (KEY_PRESSED(VK_ESCAPE))
        return Action::STOP;

    if (KEY_PRESSED(VK_SPACE))
        return Action::EXPLODE;

    if (KEY_PRESSED(VK_SHIFT))
        return Action::BOOST;

    if (KEY_PRESSED(VK_INSERT))
        return Action::PAUSE;

    if (KEY_PRESSED('R'))
        return Action::RESTORE;

    return Action::CONTINUE;
}

void startGame(HWND hndl, HANDLE handle)
{
    Snake snake(hndl, handle);

    bool isGameOver = false;
    bool paused = false;
    DWORD sleeptime = 100;
    while (!isGameOver)
    {
        Action action = processKeyBoard(snake);
        isGameOver = (action == Action::STOP || action == Action::EXPLODE);
        if (action == Action::EXPLODE)
        {
            snake.explode();
            break;
        }

        if (action == Action::BOOST)
            sleeptime = 30;
        else
            sleeptime = 100;

        if (action == Action::RESTORE)
        {
            resoreAllIconsPosition(hndl);
            break;
        }

        if (action == Action::PAUSE)
        {
            paused = !paused;
            Sleep(100);
        }

        if (!paused)
        {
            snake.move();
            snake.checkCollision();
            snake.setHeadDirection(action);
        }
        
        Sleep(sleeptime);
    }
}

std::pair<HWND, HANDLE> prepareHandles()
{
    HWND hndl = FindWindowA("Progman", NULL);
    hndl = FindWindowEx(hndl, 0, L"SHELLDLL_DefView", NULL);
    hndl = FindWindowEx(hndl, 0, L"SysListView32", NULL);

    DWORD Pi;
    GetWindowThreadProcessId(hndl, &Pi);
    HANDLE handle = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ, false, Pi);

    return { hndl, handle };
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    auto [hndl, handle] = prepareHandles();

    initialIconsState = saveState(hndl, handle);

    startGame(hndl, handle);
}
