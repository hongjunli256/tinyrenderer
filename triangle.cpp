#include"triangle.h"
mat<4, 4> Viewport, Perspective, ModelView;
void lookat(const vec3 eye, const vec3 center, const vec3 up) {
    vec3 n = normalized(eye - center);
    vec3 l = normalized(cross(up, n));
    vec3 m = normalized(cross(n, l));
    ModelView = mat<4, 4>{ {{l.x,l.y,l.z,0}, {m.x,m.y,m.z,0}, {n.x,n.y,n.z,0}, {0,0,0,1}} } *mat<4, 4>{{{1, 0, 0, -center.x}, { 0,1,0,-center.y }, { 0,0,1,-center.z }, { 0,0,0,1 }}};
    //先平移逆后旋转逆
}
void perspective(const double f) {
    Perspective = { {{1,0,0,0}, {0,1,0,0}, {0,0,1,0}, {0,0, -1 / f,1}} };
    //一个简单的透视，实际效果就是x和y进行/{1-1/f}
}

void viewport(const int x, const int y, const int width_screen, const int height_screen) {
    Viewport = { {{width_screen / 2., 0, 0, x + width_screen / 2.}, {0, height_screen / 2., 0, y + height_screen / 2.}, {0,0,1,0}, {0,0,0,1}} };
    //向窗口投射
}
vec4 rot(const vec4& v) {
    vec4 v_rot = ModelView * v;
    return v_rot;
}
//模型透视投影变换
vec4 persp(const vec4& v)
{
    vec4 v_persp = Perspective * v;
    //if (v_persp.w != 0)
        //v_persp = v_persp / v_persp.w;
    return v_persp;
}

class triangle {
public:

    vec4 dot[3];
    vec2 uv[3];
    vec4 norm[3];

    triangle(const Model& model, int face)
    {
        for (int i = 0; i < 3; i++)
        {
            //再三考虑下还是觉得，三角形处理的是透视下的点，而不是屏幕实际点更合适
            dot[i] = persp(rot(model.vert(face, i)));
            uv[i] = model.uv(face, i);
            norm[i] = model.normal(face, i);
        }
    }
    //计算是否为背面三角形
    double signed_triangle_area(vec2 screen[3]) {
        return (screen[1].x - screen[0].x) * (screen[2].y - screen[0].y) - (screen[1].y - screen[0].y) * (screen[2].x - screen[0].x);
    }
    //差值得到中间像素的uv，借用新的uv读取例如spec文件，diff文件等
    vec2 uv_gravity(double for_a, double for_b, double for_c)
    {
        return uv[2] * for_c + uv[0] * for_a + uv[1] * for_b;
    }
    //差值得到中间像素的法线
    vec4 norm_gravity(double for_a, double for_b, double for_c)
    {
        return norm[2] * for_c + norm[0] * for_a + norm[1] * for_b;
    }
private:

};
struct PhongShader {
private:
    const Model& model;
    vec4 l;
public:
    PhongShader(const vec3 light, const Model& m) : model(m) {
        l = normalized((ModelView * vec4{ light.x, light.y, light.z, 0. }));
    }

    TGAColor color(triangle& tri, const vec3 bar, mat<4, 4>modelView_invert_transpose) const {

        mat<2, 4>E = { tri.dot[1] - tri.dot[0],tri.dot[2] - tri.dot[0] };
        mat<2, 2>U = { tri.uv[1] - tri.uv[0],tri.uv[2] - tri.uv[0] };
        mat<2, 4>T = U.invert() * E;
        vec4 t0 = normalized(T[0]);
        vec4 t1 = normalized(T[1]);
        vec4 n_t = normalized((modelView_invert_transpose * tri.norm_gravity(bar[0], bar[1], bar[2])));
        mat<4, 4>D = { t0,t1,n_t,{0,0,0,1} };

        vec2 uv = tri.uv_gravity(bar[0], bar[1], bar[2]);

        vec4 n = normalized(D.transpose() * model.normal(uv));
        vec4 r = normalized(n * (n * l) * 2 - l);

        double ambient = .8;
        double diff = std::max(0., n * l);
        double spec = 3. * std::pow(std::max(r.z, 0.), 35) * static_cast<double>(model.specular(uv)[1]) / 255.0;

        TGAColor gl_FragColor = model.diffuse(uv);
        double all_light = (ambient + diff + spec);
        for (int channel : {0, 1, 2})
            gl_FragColor[channel] = std::min<int>(255, gl_FragColor[channel] * all_light);

        return gl_FragColor;
    }
};

class ToonShader{
private:
    TGAColor mColor;
    const Model& model;
    vec4 l; 
public:
    ToonShader(TGAColor color, const vec3 light, const Model& m) : model(m) {
        this->mColor = color;
        l = normalized((ModelView * vec4{ light.x, light.y, light.z, 0. })); // transform the light vector to view coordinates
    }

    TGAColor color(triangle& tri, const vec3 bar, mat<4, 4>modelView_invert_transpose) const {
        vec4 n = normalized(tri.norm_gravity(bar[0], bar[1], bar[2])); // per-vertex normal interpolation
        double diffuse = std::max(0., n * l);
        double intensity = .15 + diffuse;
        if (intensity > .66) intensity = 1;
        else if (intensity > .33) intensity = .66;
        else intensity = .33;

        TGAColor gl_FragColor;
        for (int channel : {0, 1, 2})
            gl_FragColor[channel] = std::min<int>(255, mColor[channel] * intensity);
        return gl_FragColor;                           
    }
};

inline int edge(int ax, int ay, int bx, int by, int px, int py) {
    return (bx - ax) * (py - ay) - (by - ay) * (px - ax);
}
void draw_triangle_color(triangle& tri, const PhongShader& shader, TGAImage& framebuffer, std::vector<double>& zbuffer_true, int width, int height) {

    vec4 ndc[3] = { tri.dot[0]/tri.dot[0].w, tri.dot[1]/ tri.dot[1].w, tri.dot[2]/ tri.dot[2].w };                // normalized device coordinates
    vec2 screen[3] = { (Viewport * ndc[0]).xy(), (Viewport * ndc[1]).xy(), (Viewport * ndc[2]).xy() }; // screen coordinates
    double space = tri.signed_triangle_area(screen);
    if (space < 0)
    {
        return;
    }
    int raw_bbminx = std::min(std::min((int)screen[0].x, (int)screen[1].x), (int)screen[2].x);
    int raw_bbminy = std::min(std::min((int)screen[0].y, (int)screen[1].y), (int)screen[2].y);
    int raw_bbmaxx = std::max(std::max((int)screen[0].x, (int)screen[1].x), (int)screen[2].x);
    int raw_bbmaxy = std::max(std::max((int)screen[0].y, (int)screen[1].y), (int)screen[2].y);

    int bbminx = std::max(0, raw_bbminx);
    int bbminy = std::max(0, raw_bbminy);
    int bbmaxx = std::min(width - 1, raw_bbmaxx);
    int bbmaxy = std::min(height - 1, raw_bbmaxy);

    if (bbminx > bbmaxx || bbminy > bbmaxy) {
        return;
    }
    int w = bbmaxx - bbminx + 1;
    int h = bbmaxy - bbminy + 1;

    int ax = (int)screen[0].x, ay = (int)screen[0].y;
    int bx = (int)screen[1].x, by = (int)screen[1].y;
    int cx = (int)screen[2].x, cy = (int)screen[2].y;

    int e0 = edge(ax, ay, bx, by, bbminx, bbminy);
    int e1 = edge(bx, by, cx, cy, bbminx, bbminy);
    int e2 = edge(cx, cy, ax, ay, bbminx, bbminy);

    int de0x = -(by - ay);
    int de1x = -(cy - by);
    int de2x = -(ay - cy);

    int de0y = (bx - ax);
    int de1y = (cx - bx);
    int de2y = (ax - cx);
    mat<4, 4> model_ = ModelView.invert_transpose();
#pragma omp parallel for private(ce0, ce1, ce2)
    for (int j = 0; j < h; j++) {
        int ce0 = e0 + de0y * j;
        int ce1 = e1 + de1y * j;
        int ce2 = e2 + de2y * j;
        for (int i = 0; i < w; i++) {
            bool inside =
                (ce0 >= 0 && ce1 >= 0 && ce2 >= 0) ||
                (ce0 <= 0 && ce1 <= 0 && ce2 <= 0);

            if (inside) {
                double for_c = (double)ce0 /tri.dot[2].w;
                double for_a = (double)ce1 /tri.dot[0].w;
                double for_b = (double)ce2 /tri.dot[1].w;
                vec3 bar = { for_a,for_b ,for_c };
                bar = bar / (for_a + for_b + for_c);
                TGAColor color_more_real = shader.color(tri, bar, model_);
                double z = (ndc[0].z * ce1 + ndc[1].z * ce2 + ndc[2].z * ce0) / (ce1 + ce2 + ce0);

                int idx = bbminx + i + (bbminy + j) * width;
                if (z > zbuffer_true[idx])
                {
                    framebuffer.set(bbminx + i, bbminy + j, color_more_real);
                    zbuffer_true[idx] = z;
                }
            }
            ce0 += de0x;
            ce1 += de1x;
            ce2 += de2x;
        }
    }
}
void draw_shadow_zbuffer(triangle& tri,std::vector<double>& zbuffer_true, int width, int height) {

    vec4 ndc[3] = { tri.dot[0] / tri.dot[0].w, tri.dot[1] / tri.dot[1].w, tri.dot[2] / tri.dot[2].w };                // normalized device coordinates
    vec2 screen[3] = { (Viewport * ndc[0]).xy(), (Viewport * ndc[1]).xy(), (Viewport * ndc[2]).xy() }; // screen coordinates
    double space = tri.signed_triangle_area(screen);
    if (space < 0)
    {
        return;
    }
    int raw_bbminx = std::min(std::min((int)screen[0].x, (int)screen[1].x), (int)screen[2].x);
    int raw_bbminy = std::min(std::min((int)screen[0].y, (int)screen[1].y), (int)screen[2].y);
    int raw_bbmaxx = std::max(std::max((int)screen[0].x, (int)screen[1].x), (int)screen[2].x);
    int raw_bbmaxy = std::max(std::max((int)screen[0].y, (int)screen[1].y), (int)screen[2].y);

    int bbminx = std::max(0, raw_bbminx);
    int bbminy = std::max(0, raw_bbminy);
    int bbmaxx = std::min(width - 1, raw_bbmaxx);
    int bbmaxy = std::min(height - 1, raw_bbmaxy);

    if (bbminx > bbmaxx || bbminy > bbmaxy) {
        return;
    }
    int w = bbmaxx - bbminx + 1;
    int h = bbmaxy - bbminy + 1;

    int ax = (int)screen[0].x, ay = (int)screen[0].y;
    int bx = (int)screen[1].x, by = (int)screen[1].y;
    int cx = (int)screen[2].x, cy = (int)screen[2].y;

    int e0 = edge(ax, ay, bx, by, bbminx, bbminy);
    int e1 = edge(bx, by, cx, cy, bbminx, bbminy);
    int e2 = edge(cx, cy, ax, ay, bbminx, bbminy);

    int de0x = -(by - ay);
    int de1x = -(cy - by);
    int de2x = -(ay - cy);

    int de0y = (bx - ax);
    int de1y = (cx - bx);
    int de2y = (ax - cx);
#pragma omp parallel for private(ce0, ce1, ce2)
    for (int j = 0; j < h; j++) {
        int ce0 = e0 + de0y * j;
        int ce1 = e1 + de1y * j;
        int ce2 = e2 + de2y * j;
        for (int i = 0; i < w; i++) {
            bool inside =
                (ce0 >= 0 && ce1 >= 0 && ce2 >= 0) ||
                (ce0 <= 0 && ce1 <= 0 && ce2 <= 0);

            if (inside) {
                double z = (ndc[0].z * ce1 + ndc[1].z * ce2 + ndc[2].z * ce0) / (ce1 + ce2 + ce0);

                int idx = bbminx + i + (bbminy + j) * width;
                if (z > zbuffer_true[idx])
                {
                   
                    zbuffer_true[idx] = z;
                }
            }
            ce0 += de0x;
            ce1 += de1x;
            ce2 += de2x;
        }
    }
}
void draw_triangle_toon(triangle& tri, const ToonShader& shader, TGAImage& framebuffer, std::vector<double>& zbuffer_true, int width, int height) {

    vec4 ndc[3] = { tri.dot[0] / tri.dot[0].w, tri.dot[1] / tri.dot[1].w, tri.dot[2] / tri.dot[2].w };                // normalized device coordinates
    vec2 screen[3] = { (Viewport * ndc[0]).xy(), (Viewport * ndc[1]).xy(), (Viewport * ndc[2]).xy() }; // screen coordinates
    double space = tri.signed_triangle_area(screen);
    if (space < 0)
    {
        return;
    }
    int raw_bbminx = std::min(std::min((int)screen[0].x, (int)screen[1].x), (int)screen[2].x);
    int raw_bbminy = std::min(std::min((int)screen[0].y, (int)screen[1].y), (int)screen[2].y);
    int raw_bbmaxx = std::max(std::max((int)screen[0].x, (int)screen[1].x), (int)screen[2].x);
    int raw_bbmaxy = std::max(std::max((int)screen[0].y, (int)screen[1].y), (int)screen[2].y);

    int bbminx = std::max(0, raw_bbminx);
    int bbminy = std::max(0, raw_bbminy);
    int bbmaxx = std::min(width - 1, raw_bbmaxx);
    int bbmaxy = std::min(height - 1, raw_bbmaxy);

    if (bbminx > bbmaxx || bbminy > bbmaxy) {
        return;
    }
    int w = bbmaxx - bbminx + 1;
    int h = bbmaxy - bbminy + 1;

    int ax = (int)screen[0].x, ay = (int)screen[0].y;
    int bx = (int)screen[1].x, by = (int)screen[1].y;
    int cx = (int)screen[2].x, cy = (int)screen[2].y;

    int e0 = edge(ax, ay, bx, by, bbminx, bbminy);
    int e1 = edge(bx, by, cx, cy, bbminx, bbminy);
    int e2 = edge(cx, cy, ax, ay, bbminx, bbminy);

    int de0x = -(by - ay);
    int de1x = -(cy - by);
    int de2x = -(ay - cy);

    int de0y = (bx - ax);
    int de1y = (cx - bx);
    int de2y = (ax - cx);
    mat<4, 4> model_ = ModelView.invert_transpose();
#pragma omp parallel for private(ce0, ce1, ce2)
    for (int j = 0; j < h; j++) {
        int ce0 = e0 + de0y * j;
        int ce1 = e1 + de1y * j;
        int ce2 = e2 + de2y * j;
        for (int i = 0; i < w; i++) {
            bool inside =
                (ce0 >= 0 && ce1 >= 0 && ce2 >= 0) ||
                (ce0 <= 0 && ce1 <= 0 && ce2 <= 0);

            if (inside) {
                double for_c = (double)ce0 / tri.dot[2].w;
                double for_a = (double)ce1 / tri.dot[0].w;
                double for_b = (double)ce2 / tri.dot[1].w;
                vec3 bar = { for_a,for_b ,for_c };
                bar = bar / (for_a + for_b + for_c);
                TGAColor color_more_real = shader.color(tri, bar, model_);
                double z = (ndc[0].z * ce1 + ndc[1].z * ce2 + ndc[2].z * ce0) / (ce1 + ce2 + ce0);

                int idx = bbminx + i + (bbminy + j) * width;
                if (z >= zbuffer_true[idx])
                {
                    framebuffer.set(bbminx + i, bbminy + j, color_more_real);
                    zbuffer_true[idx] = z;
                }
            }
            ce0 += de0x;
            ce1 += de1x;
            ce2 += de2x;
        }
    }
}

void drop_zbuffer(TGAImage& zbuffer_img, std::vector<double>& zbuffer_true, int width, int height) {
    double minz = +1000;
    double maxz = -1000;
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            double z = zbuffer_true[x + y * width];
            if (z < -100) continue;
            minz = std::min(z, minz);
            maxz = std::max(z, maxz);
        }
    }
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            double z = zbuffer_true[x + y * width];
            if (z < -100) continue;
            z = (z - minz) / (maxz - minz) * 255;
            unsigned char c = (unsigned char)z;
            zbuffer_img.set(x, y, { c, 255, 255, 255 });
        }
    }
}
void build_obj_triangle(const Model &model, TGAImage& framebuffer, TGAImage& zbuffer_img, TGAImage& framebuffer_toon, std::vector<double>& zbuffer_true, std::vector<double>& zbuffer_true_shadow,const RenderSettings& setting)
{
   // Model model(obj_name_str);
    //const vec3 eye = { 0,0,4 };
    //const vec3 center = { 0,0,0 };
    //const vec3 up = { 0,1,0 };
    //const vec3 light_vec = { 1,1,1 };

    lookat(setting.eye, setting.center, setting.up);
    perspective(norm(setting.eye - setting.center));
    viewport(width_obj/16, height_obj/16, width_obj*7/8, height_obj*7/8);

    PhongShader shader(setting.light_vec,model);
    for (int i = 0; i < model.nfaces(); i++)
    {
        triangle tri(model, i);
        draw_triangle_color(tri,shader,framebuffer,zbuffer_true,width_obj,height_obj);
    }
    drop_zbuffer(zbuffer_img, zbuffer_true, width_obj, height_obj);
    mat<4, 4> M = (Viewport * Perspective * ModelView).invert();

    lookat(setting.light_vec, setting.center, setting.up);
    perspective(norm(setting.eye - setting.center));
    viewport(width_obj_shadow / 16, height_obj_shadow / 16, width_obj_shadow * 7 / 8, height_obj_shadow * 7 / 8);

    mat<4, 4> N = Viewport * Perspective * ModelView;
    for (int i = 0; i < model.nfaces(); i++)
    {
        triangle tri(model, i);
        draw_shadow_zbuffer(tri,  zbuffer_true_shadow, width_obj_shadow, height_obj_shadow);
    }
    std::vector<bool> mask(width_obj * height_obj, false);
    for (int x = 0; x < width_obj; x++) {
        for (int y = 0; y < height_obj; y++) {
            vec4 fragment = M * vec4{ (double)x, (double)y, zbuffer_true[x + y * width_obj], 1. };
            vec4 q = N * fragment;
            vec3 p = q.xyz() / q.w;
            bool lit = (fragment.z < -100 ||                                   // it's the background or
                (p.x < 0 || p.x >= width_obj_shadow || p.y < 0 || p.y >= height_obj_shadow) ||   // it is out of bounds of the shadow buffer
                (p.z > zbuffer_true_shadow[int(p.x) + int(p.y) * height_obj_shadow] - .03));  // it is visible
            mask[x + y * width_obj] = lit;
        }
    }

    for (int x = 0; x < width_obj; x++) {
        for (int y = 0; y < height_obj; y++) {
            if (mask[x + y * width_obj]) continue;
            TGAColor c = framebuffer.get(x, y);
            vec3 a = { c[0], c[1], c[2] };
            if (norm(a) < 80) continue;
            a = normalized(a) * 80;
            framebuffer.set(x, y, { (unsigned char)a[0], (unsigned char)a[1], (unsigned char)a[2], 255 });
        }
    }

    ////卡通渲染
    //lookat(setting.eye, setting.center, setting.up);
    //perspective(norm(setting.eye - setting.center));
    //viewport(width_obj / 16, height_obj / 16, width_obj * 7 / 8, height_obj * 7 / 8);

    //ToonShader toonShader(orange, setting.light_vec, model);
    //for (int i = 0; i < model.nfaces(); i++)
    //{
    //    triangle tri(model, i);
    //    draw_triangle_toon(tri, toonShader, framebuffer_toon, zbuffer_true, width_obj, height_obj);
    //}
    //drop_zbuffer(zbuffer_img, zbuffer_true, width_obj, height_obj);

    ////基于z的简单边缘检测
    //constexpr double threshold = .15;
    //for (int y = 1; y < framebuffer.height() - 1; ++y) {
    //    for (int x = 1; x < framebuffer.width() - 1; ++x) {
    //        vec2 sum;
    //        for (int j = -1; j <= 1; ++j) {
    //            for (int i = -1; i <= 1; ++i) {
    //                constexpr int Gx[3][3] = { {-1,  0,  1}, {-2, 0, 2}, {-1, 0, 1} };
    //                constexpr int Gy[3][3] = { {-1, -2, -1}, { 0, 0, 0}, { 1, 2, 1} };
    //                sum = sum + vec2{
    //                    Gx[j + 1][i + 1] * zbuffer_true[x + i + (y + j) * width_obj],
    //                    Gy[j + 1][i + 1] * zbuffer_true[x + i + (y + j) * width_obj]
    //                };
    //            }
    //        }
    //        if (norm(sum) > threshold)
    //        {
    //            framebuffer_toon.set(x, y, TGAColor{ 0, 0, 0, 255 });
    //        }
    //            
    //    }
    //}
    return;
}