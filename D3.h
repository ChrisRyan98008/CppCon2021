#pragma once

#include <map>
#include <vector>
#include <functional>
#include <initializer_list>
#include <stdint.h>

/*/////////////////////////////////////////////////////////////////////
//  Supported operations
//  type correct and type coherent: vector, point, matrix, mesh
///////////////////////////////////////////////////////////////////////
//
//types:
//    vector (1x4) (xyz,w=0)            // point (1x4) (xyz,w=1)
//    matrix (4x4)
//    mesh   (poly collection)
//
//vector operators:
//    vector = vector * matrix;         // vector *= matrix;
//    vector = vector * vector;         // vector *= vector;
//    vector = Normalize(vector);
//
//point operators:
//    point  = point * matrix;          // point  *= matrix;
//    point  = point + vector;          // point  += vector;
//    vector = point - point;
//
//matrix operators:
//    matrix = matrix * matrix;         // matrix *= matrix;
//
//mesh operators (poly collection) used as a model, world, or screen
//    mesh  = mesh + mesh;              // mesh   += mesh;
//    mesh  = mesh * matrix;            // mesh   *= matrix;
//    mesh.PerspectiveDivide();
//
///////////////////////////////////////////////////////////////////////
// manipulator matrices
///////////////////////////////////////////////////////////////////////
//
//    Matrix Identity();
//    Matrix RotateX(float degrees);
//    Matrix RotateY(float degrees);
//    Matrix RotateZ(float degrees);
//    Matrix Scale(float x, float y, float z);
//    Matrix Translate(float x, float y, float z);
//
//matrix transforms
//    Matrix PointOfView(const Point& eye, const Point& target, const Vector& up);
//    Matrix FieldOfView(float fovAngle, float aspectRatio, float nearPlane, float farPlane);
//    Matrix Viewport(Rect& view, float minZ, float maxZ);
//
//    ScreenTrasnform(World& world, Rect& rect,
//                      Point eye, Point target, Vector up,
//                      float fovAngle, float nearPlane, float farPlane);
//*/

namespace D3
{
    using uint = uint32_t;

    class Matrix
    {
        union
        {
            struct { float _d[4][4]; };
            struct { float _unused[16]; } _data;
        };

    public:
        Matrix() {};
        ~Matrix() {};

        Matrix(const Matrix& rhs) : _data(rhs._data) {}

        Matrix(std::initializer_list<float[4]> list)
        {
            uint i = 0;
            for (auto& row : list)
            {
                for (uint j = 0; j < 4; j++)
                {
                    _d[i][j] = row[j];
                }
                i++;
            }
        }

        Matrix& operator = (const Matrix& rhs)
        {
            _data = rhs._data;
            return *this;
        };

        Matrix Multiply(const Matrix& rhs) const
        {
            Matrix ret;
            for (uint i = 0; i < 4; i++)
            {
                const float* srcRow = _d[i];
                float* retRow = ret[i];
                for (uint j = 0; j < 4; j++)
                {
                    float sum = {};
                    for (uint k = 0; k < 4; k++)
                    {
                        sum += srcRow[k] * rhs[k][j];
                    }
                    retRow[j] = sum;
                }
            }
            return ret;
        }

        float* operator [] (uint n) const { return (float*)_d[n]; }
    };

    namespace
    {
        const float pi = 3.1415926535897932384626433832795f;

        class Data4 // base of point & vector
        {
        protected:
            union
            {
                struct { float _x, _y, _z, _w; };
                struct { float _unused[4]; } _data;
            };

        public:
            Data4() {}
            Data4(const Data4& rhs) : _data(rhs._data) {}
            Data4(float x, float y, float z, float w = 0) : _x(x), _y(y), _z(z), _w(w) {}

            Data4& operator = (const Data4& rhs)
            {
                _data = rhs._data;
                return *this;
            }

            void Multiply(const Matrix& rhs)
            {
                Data4 d = {
                    _x * rhs[0][0] + _y * rhs[1][0] + _z * rhs[2][0] + _w * rhs[3][0],
                    _x * rhs[0][1] + _y * rhs[1][1] + _z * rhs[2][1] + _w * rhs[3][1],
                    _x * rhs[0][2] + _y * rhs[1][2] + _z * rhs[2][2] + _w * rhs[3][2],
                    _x * rhs[0][3] + _y * rhs[1][3] + _z * rhs[2][3] + _w * rhs[3][3] };
                _data = d._data;
            }

            float X() const { return _x; }
            float Y() const { return _y; }
            float Z() const { return _z; }
            float W() const { return _w; }
        };
    }

    class Vector : public Data4
    {
    public:
        Vector() {}
        Vector(const Vector& rhs) : Data4(rhs) {}
        Vector(float x, float y, float z) : Data4(x, y, z, 0) {}

        Vector CrossProduct(const Vector& rhs) const
        {
            float x = _y * rhs._z - _z * rhs._y;
            float y = _z * rhs._x - _x * rhs._z;
            float z = _x * rhs._y - _y * rhs._x;
            float len = sqrt(x * x + y * y + z * z);
            return { x / len, y / len, z / len };
        }
        Vector Normalize() const
        {
            float len = sqrt(float(_x * _x + _y * _y + _z * _z));
            return { _x / len, _y / len, _z / len };
        }
    };

    class Point : public Data4
    {
        uint hash() const
        {
            std::hash<float> floatHash;
            size_t ret = floatHash(_x);
            ret ^= ret << 1;
            ret += floatHash(_y);
            ret ^= ret << 1;
            ret += floatHash(_z);
            return uint(ret);
        }

    public:
        Point() {}
        Point(const Point& rhs) : Data4(rhs) {}
        Point(float x, float y, float z, float w = 1) : Data4(x, y, z, w) {}

        bool    operator < (const Point& rhs) const { return hash() < rhs.hash(); }

        Point  Add(const Vector& rhs) const
        {
            return Point(_x + rhs.X(), _y + rhs.Y(), _z + rhs.Z());
        }
        Vector  Subtract(const Point& rhs) const
        {
            return Vector(_x - rhs.X(), _y - rhs.Y(), _z - rhs.Z());
        }
        void    PerspectiveDivide()
        {
            _x /= _w;
            _y /= _w;
            _z /= _w;
            _w /= _w;
        }
    };

    struct Point2
    {
        float x = 0;
        float y = 0;
    };

    struct Polygon
    {
        struct
        {
            Point   p0;
            Point   p1;
            Point   p2;
            uint    i0;
            uint    i1;
            uint    i2;
        }       tripple3;
        uint    id;
        struct
        {
            Point2  p0;
            Point2  p1;
            Point2  p2;
        }       tripple2;
    };

    class PolyPoly
    {
    public:
        using VPoints = std::vector<POINT>;
        using VDWord = std::vector<DWORD>;

    private:
        VPoints _Points;
        VDWord  _PolyPoints;

    public:
        PolyPoly() {}
        void Add(VPoints& points)
        {
            _PolyPoints.push_back((DWORD)points.size());
            _Points.insert(_Points.end(), points.begin(), points.end());
        }
        void Draw(HDC hDC)
        {
            ::SelectObject(hDC, GetStockObject(WHITE_PEN));
            ::PolyPolyline(hDC, _Points.data(), _PolyPoints.data(), (DWORD)_PolyPoints.size());
        }
    };

    class Mesh
    {
        using Points   = std::vector<Point>;
        using Polygons = std::vector<Polygon>;
        using PointMap = std::map<Point, uint>;

        PointMap            _mapPoints;
        Points              _points;
        Polygons            _polygons;

    public:
        Mesh() {}
        Mesh(const std::initializer_list<Polygon> polygons)
        {
            for (auto& polygon : polygons)
            {
                AddPolygon(polygon);
            }
        }
        Mesh(const Mesh& rhs)
        {
            AddTo(rhs);
        }
        Mesh operator = (const Mesh& rhs)
        {
            _mapPoints.clear();
            _points.clear();
            _polygons.clear();

            return AddTo(rhs);
        }
        Mesh& AddTo(const Mesh& rhs)
        {
            size_t size = rhs.Count();
            for (size_t i = 0; i < size; i++)
            {
                AddPolygon(rhs[i]);
            }
            return *this;
        }

        int Count() const
            { return int(_polygons.size()); }

        void AddPolygon(const Polygon polygon)
        {
            Polygon poly = polygon;
            poly.tripple3.i0 = AddPoint(poly.tripple3.p0);
            poly.tripple3.i1 = AddPoint(poly.tripple3.p1);
            poly.tripple3.i2 = AddPoint(poly.tripple3.p2);
            _polygons.push_back(poly);
        }

        uint AddPoint(const Point& point)
        {
            auto it = _mapPoints.find(point);
            uint nDex = (uint)_points.size();
            if(it == _mapPoints.end())
            {
                _points.push_back(point);
                _mapPoints[point] = nDex;
            }
            else
            {
                nDex = it->second;
            }
            return nDex;
        }

        Polygon operator[](size_t position) const
        {
            Polygon poly = _polygons[position];
            poly.tripple3.p0 = _points[poly.tripple3.i0];
            poly.tripple3.p1 = _points[poly.tripple3.i1];
            poly.tripple3.p2 = _points[poly.tripple3.i2];
            return poly;
        }

        Mesh& Multiply(const Matrix& matrix)
        {
            _mapPoints.clear();
            for (auto& point : _points)
            {
                point.Multiply(matrix);
            }
            return *this;
        }

        void PerspectiveDivide()
        {
            for (Point& point : _points)
            {
                point.PerspectiveDivide();
            }
        }

        void ExportPolyPoly(PolyPoly& polyPoly)
        {
            size_t size = Count();
            for (size_t i = 0; i < size; i++)
            {
                Polygon polygon = operator[](i);
                PolyPoly::VPoints points;
                points.reserve(3);
                points.push_back({ int(polygon.tripple3.p0.X()), int(polygon.tripple3.p0.Y()) });
                points.push_back({ int(polygon.tripple3.p1.X()), int(polygon.tripple3.p1.Y()) });
                points.push_back({ int(polygon.tripple3.p2.X()), int(polygon.tripple3.p2.Y()) });
                points.push_back({ int(polygon.tripple3.p0.X()), int(polygon.tripple3.p0.Y()) });
                polyPoly.Add(points);
            }
        }
    };

    using Model = Mesh;
    using World = Mesh;
    using Screen = Mesh;
    using PModel = std::shared_ptr<Model>;

    class Rect : public RECT
    {
    public:
        Rect() : RECT{ 0 } {}
        Rect(const RECT& rhs) { *this == rhs; }
        bool operator ==(const RECT& rhs) const
        {
            return ((left   == rhs.left ) &&
                    (top    == rhs.top  ) &&
                    (right  == rhs.right) &&
                    (bottom == rhs.bottom));
        }
        bool operator !=(const RECT& rhs) const { return !operator==(rhs); }
        int Width()         const { return right - left; }
        int Height()        const { return bottom - top; }
        float AspectRatio() const { return float(Width()) / Height(); }
    };

    // Matrix Operators
    inline Matrix   operator *  (const Matrix& lhs, const Matrix& rhs) // matrix = matrix * matrix;
        { return lhs.Multiply(rhs); }

    inline Matrix&  operator *= (Matrix& lhs, const Matrix& rhs) // matrix *= matrix;
        { lhs = lhs.Multiply(rhs); return lhs; }

    // Vector Operators
    inline Vector   operator *  (const Vector& lhs, const Matrix& rhs) // vector = vector * matrix;
        { Vector ret = lhs; ret.Multiply(rhs); return ret; }

    inline Vector&  operator *= (Vector& lhs, const Matrix& rhs) // vector *= matrix;
        { lhs.Multiply(rhs); return lhs; }

    inline Vector   operator *  (const Vector& lhs, const Vector& rhs) // vector = vector * vector; // vector cross product
        { return lhs.CrossProduct(rhs); }

    inline Vector&  operator *= (Vector& lhs, const Vector& rhs) // vector *= vector; // vector cross product
        { lhs = lhs.CrossProduct(rhs); return lhs; }

    inline Vector   Normalize(const Vector& rhs)
        { return rhs.Normalize(); }

    // Point Operators
    inline Point    operator *  (const Point& lhs, const Matrix& rhs) // point = point * matrix;
        { Point ret = lhs; ret.Multiply(rhs); return ret; }

    inline Point&   operator *= (Point& lhs, const Matrix& rhs) // point *= matrix;
        { lhs.Multiply(rhs); return lhs; }

    inline Point    operator +  (const Point& lhs, const Vector& rhs) // point = point + vector;
        { return lhs.Add(rhs); }

    inline Point&   operator += (Point& lhs, const Vector& rhs) // point += vector;
        { lhs = lhs.Add(rhs); return lhs; }

    inline Vector   operator -  (const Point& lhs, const Point& rhs) // vector = point - point;
        { return lhs.Subtract(rhs); }

    // Mesh Operators (poly collection) used as a model, world, or screen
    inline Mesh     operator +  (const Mesh& lhs, const Mesh& rhs) // mesh = mesh + mesh;
        { Mesh ret = lhs; ret.AddTo(rhs); return ret; }

    inline Mesh&    operator += (Mesh& lhs, const Mesh& rhs) // mesh += mesh;
        { return lhs.AddTo(rhs); }

    inline Mesh     operator *  (const Mesh& lhs, const Matrix& rhs) // mesh = mesh * matrix;
    { Mesh ret = lhs; ret.Multiply(rhs); return ret; }

    inline Mesh&    operator *= (Mesh& lhs, const Matrix& rhs) // mesh *= matrix;
    { return lhs.Multiply(rhs); }

    inline Matrix Identity()
    {
        return {{ 1, 0, 0, 0},
                { 0, 1, 0, 0},
                { 0, 0, 1, 0},
                { 0, 0, 0, 1} };
    }
    inline Matrix RotateX(float rotation)
    {
        float c = cos(rotation * pi / 180);
        float s = sin(rotation * pi / 180);
        return {{ 1,  0, 0, 0 },
                { 0,  c, s, 0 },
                { 0, -s, c, 0 },
                { 0,  0, 0, 1 } };
    }
    inline Matrix RotateY(float rotation)
    {
        float c = cos(rotation * pi / 180);
        float s = sin(rotation * pi / 180);
        return {{ c, 0,-s, 0 },
                { 0, 1, 0, 0 },
                { s, 0, c, 0 },
                { 0, 0, 0, 1 } };
    }
    inline Matrix RotateZ(float  rotation)
    {
        float c = cos(rotation * pi / 180);
        float s = sin(rotation * pi / 180);
        return {{ c, s, 0, 0 },
                {-s, c, 0, 0 },
                { 0, 0, 1, 0 },
                { 0, 0, 0, 1 } };
    }
    inline Matrix Scale(float x, float y, float z)
    {
        return {{ x, 0, 0, 0 },
                { 0, y, 0, 0 },
                { 0, 0, z, 0 },
                { 0, 0, 0, 1 } };
    }
    inline Matrix Translate(float x, float y, float z)
    {
        return {{ 1, 0, 0, 0 },
                { 0, 1, 0, 0 },
                { 0, 0, 1, 0 },
                { x, y, z, 1 } };
    }

    inline Matrix PointOfView(const Point& eye, const Point& target, const Vector& up)
    {
        Vector z = Normalize(eye - target);
        Vector x = Normalize(up) * z;
        Vector y = z * x;

        return {{ x.X(), y.X(), z.X(), 0,},
                { x.Y(), y.Y(), z.Y(), 0,},
                { x.Z(), y.Z(), z.Z(), 0,},
                {   -(x.X() * eye.X() + x.Y() * eye.Y() + x.Z() * eye.Z()),
                    -(y.X() * eye.X() + y.Y() * eye.Y() + y.Z() * eye.Z()),
                    -(z.X() * eye.X() + z.Y() * eye.Y() + z.Z() * eye.Z()), 1 } };
    }
    inline Matrix FieldOfView(float fovAngle, float aspectRatio, float nearPlaneDistance, float farPlaneDistance)
    {
        float fov = 1 / tan(fovAngle * pi / 180 / 2);
        return {{fov / aspectRatio, 0, 0, 0,},
                {0, fov, 0, 0,},
                {0, 0, farPlaneDistance / (nearPlaneDistance - farPlaneDistance), -1, },
                {0, 0, (nearPlaneDistance * farPlaneDistance) / (nearPlaneDistance - farPlaneDistance), 0,} };
    }
    inline Matrix Viewport(Rect& view, float minZ, float maxZ)
    {
        float w = float(view.Width()) / 2;
        float h = float(view.Height()) / 2;
        float x = float(view.left) + w;
        float y = float(view.top) + h;
        maxZ -= minZ;
        return {{ w,  0,   0,  0, },
                { 0, -h,   0,  0, },
                { 0,  0, maxZ, 0, },
                { x,  y, minZ, 1, } };
    }
    inline Screen ScreenTrasnform(World& world, Rect& rect,
                                    Point eye,Point target, Vector up,
                                    float fovAngle, float nearPlane, float farPlane)
    {
        Matrix pov = PointOfView(eye, target, up);
        Matrix fov = FieldOfView(fovAngle, rect.AspectRatio(), nearPlane, farPlane);
        Matrix view = Viewport(rect, 0, 100);

        Screen screen = world * (pov * fov * view);
        screen.PerspectiveDivide();

        return screen;
    }

};  // namespace D3
