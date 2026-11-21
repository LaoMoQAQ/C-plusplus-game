#ifndef ANIM_H
#define ANIM_H


#include <cmath>


// ==========================================================
// 动画小工具
// ==========================================================
//
// 全部是纯函数 / 轻量结构，头文件即可，不产生额外编译单元。
// 用来做菜单淡入、选项高亮滑动、对话框上滑、立绘交叉淡入等。

namespace Anim
{

inline float Clamp01(float v)
{
    return v < 0.0f ? 0.0f : (v > 1.0f ? 1.0f : v);
}


inline float Lerp(float a, float b, float t)
{
    return a + (b - a) * t;
}


// 与帧率无关的指数逼近：
//   rate 越大越快，dt 是帧间隔（秒）。
// 高刷屏和 60Hz 下动画速度一致。
inline float Approach(float current, float target, float rate, float dt)
{
    float t = 1.0f - std::exp(-rate * dt);

    return current + (target - current) * t;
}


inline float EaseOutCubic(float t)
{
    t = Clamp01(t);

    float u = 1.0f - t;

    return 1.0f - u * u * u;
}


inline float EaseInCubic(float t)
{
    t = Clamp01(t);

    return t * t * t;
}


inline float EaseInOutCubic(float t)
{
    t = Clamp01(t);

    return t < 0.5f
        ? 4.0f * t * t * t
        : 1.0f - std::pow(-2.0f * t + 2.0f, 3.0f) * 0.5f;
}


// 轻微过冲，用在菜单弹出的"弹性"上。
inline float EaseOutBack(float t)
{
    t = Clamp01(t);

    const float c1 = 1.70158f;
    const float c3 = c1 + 1.0f;

    float u = t - 1.0f;

    return 1.0f + c3 * u * u * u + c1 * u * u;
}


// 平滑缓入缓出（用于循环呼吸感）。
inline float EaseInOutSine(float t)
{
    t = Clamp01(t);

    return -(std::cos(3.14159265f * t) - 1.0f) * 0.5f;
}

}


#endif
