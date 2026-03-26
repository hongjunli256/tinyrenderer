#pragma once
#include"model.h"
constexpr int width_obj = 800;
constexpr int height_obj = 800;
constexpr int width_obj_shadow = 2048;
constexpr int height_obj_shadow = 2048;

constexpr TGAColor white = { 255, 255, 255, 255 }; // attention, BGRA order
constexpr TGAColor green = { 0, 255,   0, 255 };
constexpr TGAColor red = { 0,   0, 255, 255 };
constexpr TGAColor blue = { 255, 128,  64, 255 };
constexpr TGAColor yellow = { 0, 255, 255, 255 };
constexpr TGAColor orange = { 0, 165, 255, 255 };

struct RenderSettings {
    vec3 eye = { 0, 0, 4 };          // 相机
    vec3 light_vec = { 1, 1, 1 };    // 光线方向
    const vec3 center = { 0,0,0 };
    const vec3 up = { 0,1,0 };
};
void build_obj_triangle(const Model&model, TGAImage& framebuffer, TGAImage& zbuffer, TGAImage& framebuffer_toon, std::vector<double>& zbuffer_true, std::vector<double>& zbuffer_true_shadow, const RenderSettings &setting);



////划线第三代，基于第二代进行性能优化,把浮点数运算替换为整数运算
//void line(int ax, int ay, int bx, int by, TGAImage& framebuffer, TGAColor color) {
//    //取x或者y中比较长的计算
//    bool steep = std::abs(ax - bx) < std::abs(ay - by);
//    if (steep)
//    {
//        std::swap(ax, ay);
//        std::swap(bx, by);
//    }
//    //保证for循环进行哈哈
//    if (ax > bx)
//    {
//        std::swap(ax, bx);
//        std::swap(ay, by);
//    }
//    int y = ay;
//    int ierror = 0;
//    for (int x = ax; x <= bx; x++)
//    {
//        if (steep)
//        {
//            framebuffer.set(y, x, color);
//        }
//        else
//        {
//            framebuffer.set(x, y, color);
//        }
//        ierror += 2 * std::abs(by - ay);
//        y += (by > ay ? 1 : -1) * (ierror > bx - ax);
//        ierror -= 2 * (bx - ax) * (ierror > bx - ax);
//    }
//}