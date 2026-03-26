//#pragma once
//#include"tgaimage.h"
//class triangle {
//public:
//
//    vec4 dot[3];
//    vec2 uv[3];
//    vec4 norm[3];
//
//    triangle(Model& model, int face)
//    {
//        for (int i = 0; i < 3; i++)
//        {
//            //再三考虑下还是觉得，三角形处理的是透视下的点，而不是屏幕实际点更合适
//            vec4 dot_t = persp(rot(model.vert(face, i)));
//            uv[i] = model.uv(face, i);
//            norm[i] = model.normal(face, i);
//        }
//    }
//    //计算是否为背面三角形
//    double signed_triangle_area() {
//        return (dot[1].x - dot[0].x) * (dot[2].y - dot[0].y) - (dot[1].y - dot[0].y) * (dot[2].x - dot[0].x);
//    }
//    //差值得到中间像素的uv，借用新的uv读取例如spec文件，diff文件等
//    vec2 uv_gravity(double for_a, double for_b, double for_c)
//    {
//        return uv[2] * for_c + uv[0] * for_a + uv[1] * for_b;
//    }
//    //差值得到中间像素的法线
//    vec4 norm_gravity(double for_a, double for_b, double for_c)
//    {
//        return norm[2] * for_c + norm[0] * for_a + norm[1] * for_b;
//    }
//private:
//
//};
//
//struct PhongShader {
//private:
//    const Model& model;
//    vec4 l;
//public:
//    PhongShader(const vec3 light, const Model& m) : model(m) {
//        l = normalized((ModelView * vec4{ light.x, light.y, light.z, 0. }));
//    }
//
//    TGAColor color(triangle& tri, const vec3 bar, mat<4, 4>modelView_invert_transpose) const {
//
//        mat<2, 4>E = { tri.dot[1] - tri.dot[0],tri.dot[2] - tri.dot[0] };
//        mat<2, 2>U = { tri.uv[1] - tri.uv[0],tri.uv[2] - tri.uv[0] };
//        mat<2, 4>T = U.invert() * E;
//        vec4 t0 = normalized(T[0]);
//        vec4 t1 = normalized(T[1]);
//        mat<4, 4> model_ = ModelView.invert_transpose();
//        vec4 n_t = normalized((model_ * tri.norm_gravity(bar[0], bar[1], bar[2])));
//        mat<4, 4>D = { t0,t1,n_t,{0,0,0,1} };
//
//        vec2 uv = tri.uv_gravity(bar[0], bar[1], bar[2]);
//
//        vec4 n = normalized(D.transpose() * model.normal(uv));
//        vec4 r = normalized(n * (n * l) * 2 - l);
//
//        double ambient = .4;
//        double diff = std::max(0., n * l);
//        double spec = 1. + 3. * std::pow(std::max(r.z, 0.), 35) * static_cast<double>(model.specular(uv)[1]) / 255.0;
//
//        TGAColor gl_FragColor = model.diffuse(uv);
//        double all_light = (ambient + diff + spec);
//        for (int channel : {0, 1, 2})
//            gl_FragColor[channel] = std::min<int>(255, gl_FragColor[channel] * all_light);
//
//        return gl_FragColor;
//    }
//};
//void rasterize(triangle& tri, const PhongShader& shader, TGAImage& framebuffer, std::vector<double>& zbuffer_true, int width, int height);
////struct IShader {
////    static TGAColor sample2D(const TGAImage& img, const vec2& uvf) {
////        return img.get(uvf[0] * img.width(), uvf[1] * img.height());
////    }
////    //virtual TGAColor color(const vec3 bar) const = 0;
////};
