#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <memory>
#include "D3_app.h"
#include "D3.h"
#include "Render.h"

#include <Mmsystem.h>
#pragma comment(lib, "winmm")

HINSTANCE hInst;

INT_PTR CALLBACK About(HWND hDlg, uint message, WPARAM wParam, LPARAM)
{
    switch(message)
    {
    case WM_COMMAND:
        if(LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
            return EndDialog(hDlg, LOWORD(wParam));
    case WM_INITDIALOG:
        return true;
    }
    return false;
}

static const Options::Mode   Modes[] = { Options::Wireframe, Options::DepthBuffer, Options::Image, };
static const Options::Model Models[] = { Options::Up, Options::Frankie, Options::Mixed, Options::Halfempty, Options::Earth, Options::Grid, };
static const Options::Delay Delays[] = { Options::fast, Options::medium, Options::slow, };
static const float          Scales[] = { 5, 10, 15, 20, 25, };
static const float         Offsets[] = { 5, 10, 15, 20, 25, };
static const char*        surfaces[] = { "Up.bmp", "Frankie.bmp", "Earth.bmp", "Grid.bmp", };

static Options             m_options = { surfaces, Scales[ID_SCALE_DEFAULT - ID_SCALE], Offsets[ID_OFFSET_DEFAULT - ID_OFFSET] };

static uint m_nDelay = ID_SPEED_DEFALT;
static uint m_nMode  = ID_MODE_DEFAULT;
static uint m_nModel = ID_MODEL_DEFAULT;
static uint m_nScale = ID_SCALE_DEFAULT;
static uint m_nOffset= ID_OFFSET_DEFAULT;

static IRender* _pRender = nullptr;
static D3::Point _eye = { 0, 0, 100, 0 };

template<typename T1, typename T2>
void OnRange(HWND hWnd, uint nID, uint& nSetting, uint idBase, T1& option, T2 values[])
{
    HMENU hMenu = GetMenu(hWnd);
    CheckMenuItem(hMenu, nSetting, MF_UNCHECKED);
    nSetting = nID;
    option = values[nSetting - idBase];
    CheckMenuItem(hMenu, nSetting, MF_CHECKED);

    InvalidateRect(hWnd, nullptr, false);
}
void OnToggle(HWND hWnd, uint id, bool& option)
{
    HMENU hMenu = GetMenu(hWnd);
    option = !option;
    CheckMenuItem(hMenu, id, option ? MF_CHECKED : MF_UNCHECKED);
    InvalidateRect(hWnd, nullptr, false);
}


LRESULT CALLBACK WndProc(HWND hWnd, uint message, WPARAM wParam, LPARAM lParam)
{
    switch(message)
    {
    case WM_CREATE:
        _pRender = IRender::Create(hWnd);
        ShowWindow(hWnd, SW_SHOW);
        break;

    case WM_COMMAND:
    {
        switch(LOWORD(wParam))
        {
        case ID_SPEED_FAST:
        case ID_SPEED_MEDIUM:
        case ID_SPEED_SLOW:
            OnRange(hWnd, LOWORD(wParam), m_nDelay, ID_SPEED, m_options.delay, Delays);
            break;

        case ID_SPEED_PAUSE:
            OnToggle(hWnd, ID_SPEED_PAUSE, m_options.pause);
            break;

        case ID_MODE_WIREFRAME:
        case ID_MODE_DEPTH_BUFFER:
        case ID_MODE_IMAGE:
            OnRange(hWnd, LOWORD(wParam), m_nMode, ID_MODE, m_options.mode, Modes);
            break;

        case ID_MODE_STATS:
            OnToggle(hWnd, ID_MODE_STATS, m_options.stats);
            break;

        case ID_MODE_TRACK:
            OnToggle(hWnd, ID_MODE_TRACK, m_options.track);
            if(m_options.track) SetTimer(hWnd, WM_TIMER, 1, nullptr);
            else                KillTimer(hWnd, WM_TIMER);
            break;

        case ID_MODEL_UP:
        case ID_MODEL_FRANKIE:
        case ID_MODEL_MIXED:
        case ID_MODEL_HALFEMPTY:
        case ID_MODEL_EARTH:
        case ID_MODEL_GRID:
            OnRange(hWnd, LOWORD(wParam), m_nModel, ID_MODEL, m_options.model, Models);
            break;

        case ID_SCALE_5:
        case ID_SCALE_10:
        case ID_SCALE_15:
        case ID_SCALE_20:
        case ID_SCALE_25:
            OnRange(hWnd, LOWORD(wParam), m_nScale, ID_SCALE, m_options.scale, Scales);
            break;

        case ID_OFFSET_5:
        case ID_OFFSET_10:
        case ID_OFFSET_15:
        case ID_OFFSET_20:
        case ID_OFFSET_25:
            OnRange(hWnd, LOWORD(wParam), m_nOffset, ID_OFFSET, m_options.offset, Offsets);
            break;

        case ID_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(ID_ABOUT), hWnd, About);
            break;

        case ID_EXIT:
            DestroyWindow(hWnd);
            break;
        }
        InvalidateRect(hWnd, nullptr, false);
        return 0;
    }

    case WM_TIMER:
        if(m_options.track)
        {
            _eye = { 0, 0, 100, 0 };
            JOYINFOEX joyInfo = { sizeof(JOYINFOEX), JOY_RETURNALL, };
            if(JOYERR_NOERROR == joyGetPosEx(0, &joyInfo))
            {
                std::swap(joyInfo.dwRpos, joyInfo.dwZpos);
                _eye = { ((float(joyInfo.dwXpos) - 32768) * 100 / 32768),
                        -((float(joyInfo.dwYpos) - 32768) * 100 / 32768),
                         ((float(joyInfo.dwZpos) - 32768) *  50 / 32768) + 100,
                        -((float(joyInfo.dwRpos) - 32768) * 180 / 32768) };
            }
        }
        break;

    case WM_MOUSEMOVE:
        if(!m_options.track)
        {
            static uint count = 0;
            if(wParam == (MK_CONTROL | MK_LBUTTON))
            {
                RECT rect;
                GetClientRect(hWnd, &rect);
                _eye = { (float)MulDiv(LOWORD(lParam), 200, rect.right) - 100,
                       -((float)MulDiv(HIWORD(lParam), 200, rect.bottom) - 100),
                        _eye.Z(), _eye.W() };
                static uint count = 0;
                if(++count % 20 == 0)
                    InvalidateRect(hWnd, nullptr, false);
            } else if(wParam == (MK_SHIFT | MK_LBUTTON))
            {
                RECT rect;
                GetClientRect(hWnd, &rect);
                _eye = { _eye.X(), _eye.Y(),
                         (float)MulDiv(HIWORD(lParam), 100, rect.bottom) + 50,
                       -((float)MulDiv(LOWORD(lParam), 360, rect.right) - 180) };
                if(++count % 20 == 0)
                    InvalidateRect(hWnd, nullptr, false);
            }
        }
        break;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        if(_pRender)
            _pRender->Draw(hdc, m_options, _eye);
        EndPaint(hWnd, &ps);
        break;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
    default:
        break;
    }
    return DefWindowProc(hWnd, message, wParam, lParam);
}

HWND AppInit()
{
    static const char windowClass[] = "D3";
    if(RegisterClassEx(&WNDCLASSEX{ sizeof(WNDCLASSEX), CS_HREDRAW | CS_VREDRAW, WndProc, 0, 0, hInst,
                        LoadIcon(hInst, MAKEINTRESOURCE(ID_ICON)), 
                        LoadCursor(nullptr, IDC_ARROW),
                        CreateSolidBrush(RGB(0, 0, 0)),
                        MAKEINTRESOURCE(ID_D3), windowClass,
                        LoadIcon(hInst, MAKEINTRESOURCE(ID_ICON)), }))
    {
        return CreateWindow(windowClass, "", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, nullptr, nullptr, hInst, nullptr);
    }
    return nullptr;
}

int Run(HWND hWnd, uint idAccel = 0)
{
    HACCEL hAccel = nullptr;
    if(idAccel)
        hAccel = LoadAccelerators(hInst, MAKEINTRESOURCE(idAccel));

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if(!hAccel ||
            !TranslateAccelerator(hWnd, hAccel, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    return (int)msg.wParam;
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int)
{
    HWND hWnd = nullptr;
    hInst = hInstance;
    int ret = 0;

    if(hWnd = AppInit())
        ret = Run(hWnd, ID_D3);

    return ret;
}
