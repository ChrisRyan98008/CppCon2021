#pragma once
#include <memory>

using uint = uint32_t;

struct Options
{
    const char** surfaces = nullptr;

    enum Mode
    {
        Wireframe,
        DepthBuffer,
        Image,
    };
    enum Model
    {
        Up,
        Frankie,
        Mixed,
        Halfempty,
        Earth,
        Grid
    };
    enum Delay
    {
        slow    = 100,
        medium  = 50,
        fast    = 1,
    };

    float   scale   = {};
    float   offset  = {};
    Delay   delay   = fast;
    Mode    mode    = Wireframe;
    Model   model   = Up;
    bool    track   = false;
    bool    stats   = false;
    bool    pause   = false;

    bool operator != (const Options& rhs) { return !operator==(rhs); }
    bool operator == (const Options& rhs)
    {
        return ((scale == rhs.scale)  &&
                (offset== rhs.offset) &&
                (delay == rhs.delay)  &&
                (mode  == rhs.mode)   &&
                (model == rhs.model)  &&
                (track == rhs.track)  &&
                (stats == rhs.stats)  &&
                (pause == rhs.pause));
    }
};

class IRender
{
public:
    static IRender* Create(HWND hWnd);

    virtual ~IRender() {}

    virtual void Timer() = 0;
    virtual void Draw(HDC hdcScreen, Options& options, D3::Point& eye) = 0;
};
