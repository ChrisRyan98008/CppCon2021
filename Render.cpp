#include <windows.h>
#include <functional>
#include <memory>
#include <map>

#include "D3.h"
#include "Render.h"
#include "D3_app.h"

using namespace D3;

World CreateWorld(Model& model, float angle, float scale, float offset)
{
    Model modelX = model  * Scale(scale, scale, scale);
    Model modelY = modelX * RotateZ(90);
    Model modelZ = modelX * RotateY(90);

    World world = modelX * (RotateX(angle) *                              Translate(-offset, -offset, -20));
    world += modelY * (RotateY(angle) *                                   Translate(-offset,  offset, -40));
    world += modelZ * (RotateZ(angle) *                                   Translate( offset, -offset, -60));
    world += modelX * (RotateX(angle) * RotateY(angle) * RotateZ(angle) * Translate( offset,  offset,   0));

    return world;
}

PModel MakeUp()
{
    return std::make_shared<Model>(Model({
                { { { -1, -1,  1 }, { -1,  1,  1 }, {  1,  1,  1 } }, ID_UP, { {    0,    0 }, {    0,  179 }, {  179,  179 } } },
                { { {  1,  1,  1 }, {  1, -1,  1 }, { -1, -1,  1 } }, ID_UP, { {  179,  179 }, {  179,   0 }, {    0,    0 } } },
                { { { -1, -1, -1 }, { -1,  1, -1 }, {  1,  1, -1 } }, ID_UP, { {    0,    0 }, {    0, 179 }, {  179,  179 } } },
                { { {  1,  1, -1 }, {  1, -1, -1 }, { -1, -1, -1 } }, ID_UP, { {  179,  179 }, {  179,   0 }, {    0,    0 } } },
                { { { -1,  1, -1 }, { -1,  1,  1 }, {  1,  1,  1 } }, ID_UP, { {    0,    0 }, {    0, 179 }, {  179,  179 } } },
                { { {  1,  1,  1 }, {  1,  1, -1 }, { -1,  1, -1 } }, ID_UP, { {  179,  179 }, {  179,   0 }, {    0,    0 } } },
                { { { -1, -1, -1 }, { -1, -1,  1 }, {  1, -1,  1 } }, ID_UP, { {    0,    0 }, {    0, 179 }, {  179,  179 } } },
                { { {  1, -1,  1 }, {  1, -1, -1 }, { -1, -1, -1 } }, ID_UP, { {  179,  179 }, {  179,   0 }, {    0,    0 } } },
                { { {  1, -1, -1 }, {  1, -1,  1 }, {  1,  1,  1 } }, ID_UP, { {    0,    0 }, {    0, 179 }, {  179,  179 } } },
                { { {  1,  1,  1 }, {  1,  1, -1 }, {  1, -1, -1 } }, ID_UP, { {  179,  179 }, {  179,   0 }, {    0,    0 } } },
                { { { -1, -1, -1 }, { -1, -1,  1 }, { -1,  1,  1 } }, ID_UP, { {    0,    0 }, {    0, 179 }, {  179,  179 } } },
                { { { -1,  1,  1 }, { -1,  1, -1 }, { -1, -1, -1 } }, ID_UP, { {  179,  179 }, {  179,   0 }, {    0,    0 } } },
            }));
}

PModel MakeFrankie()
{
    return std::make_shared<Model>(Model({
                { { { -1, -1,  1 }, { -1,  1,  1 }, {  1,  1,  1 } }, ID_FRANKIE, { {    0,    0 }, {    0, 179 }, {  179,  179 } } },
                { { {  1,  1,  1 }, {  1, -1,  1 }, { -1, -1,  1 } }, ID_FRANKIE, { {  179,  179 }, {  179,   0 }, {    0,    0 } } },
                { { { -1, -1, -1 }, { -1,  1, -1 }, {  1,  1, -1 } }, ID_FRANKIE, { {    0,    0 }, {    0, 179 }, {  179,  179 } } },
                { { {  1,  1, -1 }, {  1, -1, -1 }, { -1, -1, -1 } }, ID_FRANKIE, { {  179,  179 }, {  179,   0 }, {    0,    0 } } },
                { { { -1,  1, -1 }, { -1,  1,  1 }, {  1,  1,  1 } }, ID_FRANKIE, { {    0,    0 }, {    0, 179 }, {  179,  179 } } },
                { { {  1,  1,  1 }, {  1,  1, -1 }, { -1,  1, -1 } }, ID_FRANKIE, { {  179,  179 }, {  179,   0 }, {    0,    0 } } },
                { { { -1, -1, -1 }, { -1, -1,  1 }, {  1, -1,  1 } }, ID_FRANKIE, { {    0,    0 }, {    0, 179 }, {  179,  179 } } },
                { { {  1, -1,  1 }, {  1, -1, -1 }, { -1, -1, -1 } }, ID_FRANKIE, { {  179,  179 }, {  179,   0 }, {    0,    0 } } },
                { { {  1, -1, -1 }, {  1, -1,  1 }, {  1,  1,  1 } }, ID_FRANKIE, { {    0,    0 }, {    0, 179 }, {  179,  179 } } },
                { { {  1,  1,  1 }, {  1,  1, -1 }, {  1, -1, -1 } }, ID_FRANKIE, { {  179,  179 }, {  179,   0 }, {    0,    0 } } },
                { { { -1, -1, -1 }, { -1, -1,  1 }, { -1,  1,  1 } }, ID_FRANKIE, { {    0,    0 }, {    0, 179 }, {  179,  179 } } },
                { { { -1,  1,  1 }, { -1,  1, -1 }, { -1, -1, -1 } }, ID_FRANKIE, { {  179,  179 }, {  179,   0 }, {    0,    0 } } },
        }));
}

PModel MakeMixed()
{
    return std::make_shared<Model>(Model({
                { { { -1, -1,  1 }, { -1,  1,  1 }, {  1,  1,  1 } }, ID_FRANKIE, { {    0,    0 }, {    0, 179 }, {  179,  179 } } },
                { { {  1,  1,  1 }, {  1, -1,  1 }, { -1, -1,  1 } }, ID_UP,      { {  179,  179 }, {  179,   0 }, {    0,    0 } } },
                { { { -1, -1, -1 }, { -1,  1, -1 }, {  1,  1, -1 } }, ID_FRANKIE, { {    0,    0 }, {    0, 179 }, {  179,  179 } } },
                { { {  1,  1, -1 }, {  1, -1, -1 }, { -1, -1, -1 } }, ID_UP,      { {  179,  179 }, {  179,   0 }, {    0,    0 } } },
                { { { -1,  1, -1 }, { -1,  1,  1 }, {  1,  1,  1 } }, ID_FRANKIE, { {    0,    0 }, {    0, 179 }, {  179,  179 } } },
                { { {  1,  1,  1 }, {  1,  1, -1 }, { -1,  1, -1 } }, ID_UP,      { {  179,  179 }, {  179,   0 }, {    0,    0 } } },
                { { { -1, -1, -1 }, { -1, -1,  1 }, {  1, -1,  1 } }, ID_FRANKIE, { {    0,    0 }, {    0, 179 }, {  179,  179 } } },
                { { {  1, -1,  1 }, {  1, -1, -1 }, { -1, -1, -1 } }, ID_UP,      { {  179,  179 }, {  179,   0 }, {    0,    0 } } },
                { { {  1, -1, -1 }, {  1, -1,  1 }, {  1,  1,  1 } }, ID_FRANKIE, { {    0,    0 }, {    0, 179 }, {  179,  179 } } },
                { { {  1,  1,  1 }, {  1,  1, -1 }, {  1, -1, -1 } }, ID_UP,      { {  179,  179 }, {  179,   0 }, {    0,    0 } } },
                { { { -1, -1, -1 }, { -1, -1,  1 }, { -1,  1,  1 } }, ID_FRANKIE, { {    0,    0 }, {    0, 179 }, {  179,  179 } } },
                { { { -1,  1,  1 }, { -1,  1, -1 }, { -1, -1, -1 } }, ID_UP,      { {  179,  179 }, {  179,   0 }, {    0,    0 } } },
        }));
}

PModel MakeHalfempty()
{
    return std::make_shared<Model>(Model({
                { { { -1, -1,  1 }, { -1,  1,  1 }, {  1,  1,  1 } }, ID_FRANKIE, { {    0,    0 }, {    0, 179 }, {  179,  179 } } },
                { { { -1, -1, -1 }, { -1,  1, -1 }, {  1,  1, -1 } }, ID_FRANKIE, { {    0,    0 }, {    0, 179 }, {  179,  179 } } },
                { { { -1,  1, -1 }, { -1,  1,  1 }, {  1,  1,  1 } }, ID_FRANKIE, { {    0,    0 }, {    0, 179 }, {  179,  179 } } },
                { { { -1, -1, -1 }, { -1, -1,  1 }, {  1, -1,  1 } }, ID_FRANKIE, { {    0,    0 }, {    0, 179 }, {  179,  179 } } },
                { { {  1, -1, -1 }, {  1, -1,  1 }, {  1,  1,  1 } }, ID_FRANKIE, { {    0,    0 }, {    0, 179 }, {  179,  179 } } },
                { { { -1, -1, -1 }, { -1, -1,  1 }, { -1,  1,  1 } }, ID_FRANKIE, { {    0,    0 }, {    0, 179 }, {  179,  179 } } },
        }));
}

PModel MakeEarth()
{
    return std::make_shared<Model>(Model({
                { { {  1,  1,  1 }, { -1,  1,  1 }, { -1,  1, -1 } }, ID_EARTH, { { 200, 400 }, { 200, 600 }, { 400, 600 } } },
                { { { -1,  1, -1 }, {  1,  1, -1 }, {  1,  1,  1 } }, ID_EARTH, { { 400, 600 }, { 400, 400 }, { 200, 400 } } },
                { { { -1, -1,  1 }, { -1,  1,  1 }, {  1,  1,  1 } }, ID_EARTH, { {   0, 200 }, {   0, 400 }, { 200, 400 } } },
                { { {  1,  1,  1 }, {  1, -1,  1 }, { -1, -1,  1 } }, ID_EARTH, { { 200, 400 }, { 200, 200 }, {   0, 200 } } },
                { { {  1, -1, -1 }, {  1, -1,  1 }, {  1,  1,  1 } }, ID_EARTH, { { 400, 200 }, { 200, 200 }, { 200, 400 } } },
                { { {  1,  1,  1 }, {  1,  1, -1 }, {  1, -1, -1 } }, ID_EARTH, { { 200, 400 }, { 400, 400 }, { 400, 200 } } },
                { { {  1, -1, -1 }, { -1,  1, -1 }, {  1,  1, -1 } }, ID_EARTH, { { 400, 200 }, { 600, 400 }, { 400, 400 } } },
                { { { -1,  1, -1 }, { -1, -1, -1 }, {  1, -1, -1 } }, ID_EARTH, { { 600, 400 }, { 600, 200 }, { 400, 200 } } },
                { { { -1, -1,  1 }, {  1, -1,  1 }, {  1, -1, -1 } }, ID_EARTH, { { 200,   0 }, { 200, 200 }, { 400, 200 } } },
                { { {  1, -1, -1 }, { -1, -1, -1 }, { -1, -1,  1 } }, ID_EARTH, { { 400, 200 }, { 400,   0 }, { 200,   0 } } },
                { { { -1, -1, -1 }, { -1, -1,  1 }, { -1,  1,  1 } }, ID_EARTH, { { 600, 200 }, { 800, 200 }, { 800, 400 } } },
                { { { -1,  1,  1 }, { -1,  1, -1 }, { -1, -1, -1 } }, ID_EARTH, { { 800, 400 }, { 600, 400 }, { 600, 200 } } },
        }));
}

PModel MakeGrid()
{
    return std::make_shared<Model>(Model({
                { { { -1, -1,  1 }, { -1,  1,  1 }, {  1,  1,  1 } }, ID_GRID, { {   0,   0 }, {  0, 200 }, { 200, 200 } } },
                { { {  1,  1,  1 }, {  1, -1,  1 }, { -1, -1,  1 } }, ID_GRID, { { 200, 200 }, { 200,  0 }, {   0,   0 } } },
        }));
}

PModel GetModel(Options::Model model)
{
    using Map = std::map<Options::Model, PModel>;
    static Map s_mapModels;

    auto it = s_mapModels.find(model);
    if(it != s_mapModels.end())
    {
        return it->second;
    }

    PModel ret;
    switch(model)
    {
    default:
    case Options::Up:        ret = MakeUp();        break;
    case Options::Frankie:   ret = MakeFrankie();   break;
    case Options::Mixed:     ret = MakeMixed();     break;
    case Options::Halfempty: ret = MakeHalfempty(); break;
    case Options::Earth:     ret = MakeEarth();     break;
    case Options::Grid:      ret = MakeGrid();      break;
    }
    s_mapModels[model] = ret;
    return ret;
}

class Render : public IRender
{
    friend IRender;

    HWND    m_hWnd    = nullptr;
    float   m_angle   = {};
    Rect    m_rect    = {};
    uint64_t m_nPixels= {};
    uint    m_size    = {};
    uint    m_nFrames = {};
    uint    m_nStart  = {};
    Options m_options = {};

    using PQuad = std::shared_ptr<RGBQUAD[]>;
    PQuad   m_image;

    using PUint = std::shared_ptr<uint[]>;
    PUint   m_depth;

    struct SurfaceInfo
    {
        PQuad   surface;
        int     width;
        int     height;
    };

    using MapSurfaces = std::map<uint, SurfaceInfo>;
    MapSurfaces s_mapSurfaces;

    Render(HWND hWnd) : m_hWnd(hWnd), m_nStart(GetTickCount()) { ::SetTimer(m_hWnd, (UINT_PTR)this, 1, TimerProc); }
    static void CALLBACK TimerProc(HWND hwnd, UINT uMsg, UINT_PTR event, DWORD dwTime) { ((IRender*)event)->Timer(); }
    virtual void Timer() { m_angle += 1; InvalidateRect(m_hWnd, nullptr, false); }
    virtual void Draw(HDC hdcScreen, Options& options, Point& eye);
private:
    PQuad   GetSurface(uint id, int& height, int& width);
    void    RenderWireFrame(Mesh& mesh, HDC hDepth);
    void    RenderBitmaps(const Mesh& mesh, uint* depth, RGBQUAD* image, uint& min, uint& max);
    void    GrayScale(uint* depth, uint size, uint min, uint max);
    void    DrawStats(HDC hdc, COLORREF color, Point& eye, bool doMPixels);
};

IRender* IRender::Create(HWND hWnd)
{
    return new Render(hWnd);
}

Render::PQuad Render::GetSurface(uint id, int& height, int& width)
{
    auto it = s_mapSurfaces.find(id);
    if(it != s_mapSurfaces.end())
    {
        SurfaceInfo& info = it->second;
        height = info.height;
        width = info.width;
        return info.surface;
    }
    struct
    {
        BITMAPINFO bmi;
        RGBQUAD rgb[2];
    } info = { sizeof(BITMAPINFOHEADER) };

    HDC hdc = CreateCompatibleDC(nullptr);
    HBITMAP hBmp = (HBITMAP)LoadImage(nullptr, m_options.surfaces[id - ID_SURFACES], IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE | LR_SHARED | LR_LOADFROMFILE);
    int rc = GetDIBits(hdc, hBmp, 0, 0, nullptr, &info.bmi, BI_RGB);
    PQuad surface(new RGBQUAD[info.bmi.bmiHeader.biSizeImage / sizeof(RGBQUAD)]);

    rc = GetDIBits(hdc, hBmp, 0, info.bmi.bmiHeader.biHeight, surface.get(), &info.bmi, BI_RGB);

    s_mapSurfaces[id] = { surface, info.bmi.bmiHeader.biWidth, info.bmi.bmiHeader.biHeight };

    DeleteObject(hBmp);
    DeleteDC(hdc);

    return surface;
}

void Render::RenderWireFrame(Mesh& mesh, HDC hDepth)
{
    PolyPoly polyPoly;
    mesh.ExportPolyPoly(polyPoly);
    polyPoly.Draw(hDepth);
}

void Render::RenderBitmaps(const Mesh& mesh, uint* depth, RGBQUAD* image, uint& min, uint& max)
{
    int count = mesh.Count();
    for (int i = 0; i < count; i++)
    {
        D3::Polygon polygon = mesh[i];

        int sHeight = 0;
        int sWidth = 0;
        PQuad surface = GetSurface(polygon.id, sHeight, sWidth);

        Point& p0 = polygon.tripple3.p0;
        Point& p1 = polygon.tripple3.p1;
        Point& p2 = polygon.tripple3.p2;

        Point2& s0 = polygon.tripple2.p0;
        Point2& s1 = polygon.tripple2.p1;
        Point2& s2 = polygon.tripple2.p2;

        if(p0.Y() > p1.Y()) { std::swap(p0, p1); std::swap(s0, s1); }
        if(p0.Y() > p2.Y()) { std::swap(p0, p2); std::swap(s0, s2); }
        if(p1.Y() > p2.Y()) { std::swap(p1, p2); std::swap(s1, s2); }

        uint64_t& nPixels = m_nPixels;
        auto Rasterize = [&, rect = m_rect, surface = surface.get()](Point& p0, Point& p1, Point& p2, Point& p3, Point2& s0, Point2& s1, Point2& s2, Point2& s3)
        {
            int width = rect.Width();
            int den0 = int(p2.Y() - p0.Y());
            int den1 = int(p3.Y() - p1.Y());
            for (int y = int(p0.Y()); y < int(p2.Y()); y++)
            {
                if((y < rect.top) || (y >= rect.bottom)) continue;

                int num0  = int(y - p0.Y());
                int num1  = int(y - p1.Y());
                int h0    = int(p0.X());
                int h1    = int(p1.X());
                float d0  = p0.Z();
                float d1  = p1.Z();
                float s0x = s0.x;
                float s0y = s0.y;
                float s1x = s1.x;
                float s1y = s1.y;

                if(den0 != 0)
                {
                    h0  += int((p2.X() - p0.X()) * num0 / den0);
                    d0  +=    ((p2.Z() - p0.Z()) * num0 / den0);
                    s0x +=    ((s2.x   - s0.x)   * num0 / den0);
                    s0y +=    ((s2.y   - s0.y)   * num0 / den0);
                }
                if(den1 != 0)
                {
                    h1  += int((p3.X() - p1.X()) * num1 / den1);
                    d1  +=    ((p3.Z() - p1.Z()) * num1 / den1);
                    s1x +=    ((s3.x   - s1.x)   * num1 / den1);
                    s1y +=    ((s3.y   - s1.y)   * num1 / den1);
                }
                if(h0 > h1)
                {
                    std::swap(h0,  h1);
                    std::swap(d0,  d1);
                    std::swap(s0x, s1x);
                    std::swap(s0y, s1y);
                }

                int den = h1 - h0;
                for (int x = h0; x < h1; x++)
                {
                    if((x < rect.left) || (x >= rect.right)) continue;

                    int num  = x - h0;
                    float d  = d0;
                    float sx = s0x;
                    float sy = s0y;

                    if(den != 0)
                    {
                        d  += (d1  - d0)  * num / den;
                        sx += (s1x - s0x) * num / den;
                        sy += (s1y - s0y) * num / den;
                    }

                    uint dd = uint(d * 10000);
                    uint ndex = x + y * width;
                    uint& dep = depth[ndex];
                    if(dd < dep)
                    {
                        if(min > dd) min = dd;
                        if(max < dd) max = dd;
                        dep = dd;
                        if(image && (int(sy) < sHeight))
                        {
                            image[ndex] = surface[int(sx) + int(sy) * sWidth];
                        }
                    }
                    nPixels++;
                }
            }
        };

        Rasterize(p0, p0, p1, p2, s0, s0, s1, s2);
        Rasterize(p1, p0, p2, p2, s1, s0, s2, s2);
    }
}

void Render::GrayScale(uint* depth, uint size, uint min, uint max)
{
    int diff = max - min;
    if(!diff)
        return;

    for (uint* pixel = depth; pixel< depth + size; pixel++)
    {
        if(*pixel <= max)
            *pixel = (*pixel - min) * 255 / diff * 0x010101;
    }
}

void Render::DrawStats(HDC hdc, COLORREF color, Point& eye, bool doMPixels)
{
    if(m_options.stats)
    {
        m_nFrames++;
        char sz[30] = {};
        SetTextColor(hdc, color);
        SetBkColor(hdc, 0xffffff - color);
        if(!m_options.pause)
        {
            int offset = 0;
            uint delta = GetTickCount() - m_nStart + 1;
            int len = sprintf(sz, "Frames/S = %f", double(m_nFrames) * 1000 / delta);
            TextOut(hdc, 0, offset, sz, len);
            offset += 20;
            if(doMPixels)
            {
                len = sprintf(sz, "MPixels/S = %f", double(m_nPixels) / 1000 / delta);
                TextOut(hdc, 0, offset, sz, len);
                offset += 20;
            }
            len = sprintf(sz, "pov: x:%-2d y:%-2d z:%-2d r:%-2d", (int)eye.X(), (int)eye.Y(), (int)eye.Z(), (int)eye.W());
            TextOut(hdc, 0, offset, sz, len);
            offset += 20;
        }
        else
        {
            TextOutA(hdc, 0, 0, "Paused", 6);
        }
    }
}

void Render::Draw(HDC hdcScreen, Options& options, Point& eye)
{
    GetClientRect(m_hWnd, &m_rect);
    uint height = m_rect.Height();
    uint width  = m_rect.Width();
    uint size   = width * height;

    if(m_options != options)
    {
        m_nPixels = 0;
        m_nFrames = 0;
        m_nStart  = GetTickCount();

        m_options = options;
        if(m_options.pause) { ::KillTimer(m_hWnd, (UINT_PTR)this); }
        else                { ::SetTimer(m_hWnd, (UINT_PTR)this, m_options.delay, TimerProc); }
    }

    if((m_size != size) || !m_depth || !m_image)
    {
        m_nPixels = 0;
        m_nFrames = 0;
        m_nStart  = GetTickCount();

        m_size    = size;
        PUint depth(new uint[size]);
        PQuad image(new RGBQUAD[size]);
        m_depth = depth;
        m_image = image;
    }

    PModel pModel = GetModel(m_options.model);
    World  world  = CreateWorld(*pModel, m_angle, m_options.scale, m_options.offset);
    Screen screen = ScreenTrasnform(world, m_rect, { eye.X(), eye.Y(), eye.Z() }, { 0, 0, 0 },
                                    { (float)sin(eye.W() / 180 * pi), (float)cos(eye.W() / 180 * pi), 0 }, 45, 1, 100);
    uint     max = 0;
    uint     min = UINT_MAX;
    HBITMAP  hBitmap = nullptr;
    RGBQUAD* image = m_image.get();
    uint*    depth = m_depth.get();
    HDC      hdc = CreateCompatibleDC(nullptr);
    bool     bDrawStats = true;
    COLORREF rgbBG = RGB(255, 255, 255);

    switch(m_options.mode)
    {
    default:
    case Options::Wireframe:
        bDrawStats = false;
        BitBlt(hdc, 0, 0, width, height, hdc, 0, 0, BLACKNESS);

        hBitmap = CreateCompatibleBitmap(hdc, width, height);
        SelectObject(hdc, hBitmap);

        RenderWireFrame(screen, hdc);
        break;

    case Options::DepthBuffer:
        rgbBG = RGB(0, 0, 0);

        memset(depth, 0xff, size * sizeof(*depth));
        RenderBitmaps(screen, depth, nullptr, min, max);
        GrayScale(depth, size, min, max);

        hBitmap = CreateBitmap(width, height, 1, 32, depth);
        break;

    case Options::Image:
        memset(depth, 0xff, size * sizeof(*depth));
        memset(image, 0x00, size * sizeof(*image));
        RenderBitmaps(screen, depth, image, min, max);

        hBitmap = CreateBitmap(width, height, 1, 32, image);
        break;
    }
    SelectObject(hdc, hBitmap);
    DrawStats(hdc, rgbBG, eye, bDrawStats);
    BitBlt(hdcScreen, 0, 0, width, height, hdc, 0, 0, SRCCOPY);
    DeleteObject(hBitmap);
    DeleteDC(hdc);
}
