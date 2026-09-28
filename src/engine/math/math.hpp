#pragma once
#include <cmath>
#include <algorithm>

namespace cry {

struct Vec3 {
    float x{}, y{}, z{};

    constexpr Vec3() = default;
    constexpr Vec3(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}

    Vec3 operator+(const Vec3& r) const { return {x+r.x,y+r.y,z+r.z}; }
    Vec3 operator-(const Vec3& r) const { return {x-r.x,y-r.y,z-r.z}; }
    Vec3 operator*(float s) const { return {x*s,y*s,z*s}; }
    Vec3& operator+=(const Vec3& r) { x+=r.x; y+=r.y; z+=r.z; return *this; }
    Vec3& operator-=(const Vec3& r) { x-=r.x; y-=r.y; z-=r.z; return *this; }

    float length() const { return std::sqrt(x*x+y*y+z*z); }
    Vec3 normalized() const {
        const float l = length();
        return l > 0.00001f ? *this * (1.0f/l) : Vec3{};
    }

    static float dot(const Vec3& a, const Vec3& b) {
        return a.x*b.x + a.y*b.y + a.z*b.z;
    }

    static Vec3 cross(const Vec3& a, const Vec3& b) {
        return {
            a.y*b.z-a.z*b.y,
            a.z*b.x-a.x*b.z,
            a.x*b.y-a.y*b.x
        };
    }
};

struct Mat4 {
    float m[16]{};

    static Mat4 identity() {
        Mat4 r{};
        r.m[0]=r.m[5]=r.m[10]=r.m[15]=1.0f;
        return r;
    }

    static Mat4 translation(const Vec3& v) {
        Mat4 r = identity();
        r.m[12]=v.x; r.m[13]=v.y; r.m[14]=v.z;
        return r;
    }

    static Mat4 scale(const Vec3& v) {
        Mat4 r{};
        r.m[0]=v.x; r.m[5]=v.y; r.m[10]=v.z; r.m[15]=1.0f;
        return r;
    }

    static Mat4 rotationY(float a) {
        Mat4 r = identity();
        const float c=std::cos(a), s=std::sin(a);
        r.m[0]=c; r.m[2]=-s; r.m[8]=s; r.m[10]=c;
        return r;
    }

    static Mat4 perspective(float fov, float aspect, float nearZ, float farZ) {
        Mat4 r{};
        const float t=1.0f/std::tan(fov*0.5f);
        r.m[0]=t/aspect;
        r.m[5]=t;
        r.m[10]=-(farZ+nearZ)/(farZ-nearZ);
        r.m[11]=-1.0f;
        r.m[14]=-(2.0f*farZ*nearZ)/(farZ-nearZ);
        return r;
    }

    static Mat4 lookAt(const Vec3& eye, const Vec3& center, const Vec3& up) {
        const Vec3 f=(center-eye).normalized();
        const Vec3 s=Vec3::cross(f,up).normalized();
        const Vec3 u=Vec3::cross(s,f);
        Mat4 r=identity();
        r.m[0]=s.x; r.m[4]=s.y; r.m[8]=s.z;
        r.m[1]=u.x; r.m[5]=u.y; r.m[9]=u.z;
        r.m[2]=-f.x; r.m[6]=-f.y; r.m[10]=-f.z;
        r.m[12]=-Vec3::dot(s,eye);
        r.m[13]=-Vec3::dot(u,eye);
        r.m[14]= Vec3::dot(f,eye);
        return r;
    }
};

inline Mat4 operator*(const Mat4& a, const Mat4& b) {
    Mat4 r{};
    for(int c=0;c<4;c++)
        for(int row=0;row<4;row++)
            r.m[c*4+row] =
                a.m[0*4+row]*b.m[c*4+0] +
                a.m[1*4+row]*b.m[c*4+1] +
                a.m[2*4+row]*b.m[c*4+2] +
                a.m[3*4+row]*b.m[c*4+3];
    return r;
}

}