//#include"shader.h"
//#include"model.h"
//
//mat<4, 4> Viewport, Perspective, ModelView;
////std::vector<double> zbuffer;               // depth buffer
//void lookat(const vec3 eye, const vec3 center, const vec3 up) {
//    vec3 n = normalized(eye - center);
//    vec3 l = normalized(cross(up, n));
//    vec3 m = normalized(cross(n, l));
//    ModelView = mat<4, 4>{ {{l.x,l.y,l.z,0}, {m.x,m.y,m.z,0}, {n.x,n.y,n.z,0}, {0,0,0,1}} } *mat<4, 4>{{{1, 0, 0, -center.x}, { 0,1,0,-center.y }, { 0,0,1,-center.z }, { 0,0,0,1 }}};
//    //先平移逆后旋转逆
//}
//void perspective(const double f) {
//    Perspective = { {{1,0,0,0}, {0,1,0,0}, {0,0,1,0}, {0,0, -1 / f,1}} };
//    //一个简单的透视，实际效果就是x和y进行/{1-1/f}
//}
//
//void viewport(const int x, const int y, const int width_screen, const int height_screen) {
//    Viewport = { {{width_screen / 2., 0, 0, x + width_screen / 2.}, {0, height_screen / 2., 0, y + height_screen / 2.}, {0,0,255,0}, {0,0,0,1}} };
//    //向窗口投射
//}
//vec4 rot(const vec4& v) {
//    vec4 v_rot = ModelView * v;
//    return v_rot;
//}
////模型透视投影变换
//vec4 persp(const vec4& v)
//{
//    vec4 v_persp = Perspective * v;
//    if (v_persp.w != 0)
//        v_persp = v_persp / v_persp.w;
//    return v_persp;
//}
//
////void rasterize(const Triangle& clip, const IShader& shader, TGAImage& framebuffer) {
////    vec4 ndc[3] = { clip[0] / clip[0].w, clip[1] / clip[1].w, clip[2] / clip[2].w };                // normalized device coordinates
////    vec2 screen[3] = { (Viewport * ndc[0]).xy(), (Viewport * ndc[1]).xy(), (Viewport * ndc[2]).xy() }; // screen coordinates
////
////    mat<3, 3> ABC = { { {screen[0].x, screen[0].y, 1.}, {screen[1].x, screen[1].y, 1.}, {screen[2].x, screen[2].y, 1.} } };
////    if (ABC.det() < 1) return; // backface culling + discarding triangles that cover less than a pixel
////
////    auto [bbminx, bbmaxx] = std::minmax({ screen[0].x, screen[1].x, screen[2].x }); // bounding box for the triangle
////    auto [bbminy, bbmaxy] = std::minmax({ screen[0].y, screen[1].y, screen[2].y }); // defined by its top left and bottom right corners
////#pragma omp parallel for
////    for (int x = std::max<int>(bbminx, 0); x <= std::min<int>(bbmaxx, framebuffer.width() - 1); x++) {         // clip the bounding box by the screen
////        for (int y = std::max<int>(bbminy, 0); y <= std::min<int>(bbmaxy, framebuffer.height() - 1); y++) {
////            vec3 bc_screen = ABC.invert_transpose() * vec3 { static_cast<double>(x), static_cast<double>(y), 1. }; // barycentric coordinates of {x,y} w.r.t the triangle
////            vec3 bc_clip = { bc_screen.x / clip[0].w, bc_screen.y / clip[1].w, bc_screen.z / clip[2].w };     // check https://github.com/ssloy/tinyrenderer/wiki/Technical-difficulties-linear-interpolation-with-perspective-deformations
////            bc_clip = bc_clip / (bc_clip.x + bc_clip.y + bc_clip.z);
////            if (bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0) continue;                                                    // negative barycentric coordinate => the pixel is outside the triangle
////            double z = bc_screen * vec3{ ndc[0].z, ndc[1].z, ndc[2].z };  // linear interpolation of the depth
////            if (z <= zbuffer[x + y * framebuffer.width()]) continue;   // discard fragments that are too deep w.r.t the z-buffer
////            auto [discard, color] = shader.fragment(bc_clip);
////            if (discard) continue;                                 // fragment shader can discard current fragment
////            zbuffer[x + y * framebuffer.width()] = z;                  // update the z-buffer
////            framebuffer.set(x, y, color);                          // update the framebuffer
////        }
////    }
////}
//
////class triangle {
////public:
////    
////    vec4 dot[3];
////    vec2 uv[3];
////    vec4 norm[3];
////
////    triangle(Model& model, int face)
////    {
////        for (int i = 0; i < 3; i++)
////        {
////            //再三考虑下还是觉得，三角形处理的是透视下的点，而不是屏幕实际点更合适
////            vec4 dot_t = persp(rot(model.vert(face, i)));
////            uv[i] = model.uv(face, i);
////            norm[i] = model.normal(face, i);
////        }
////    }
////    //计算是否为背面三角形
////    double signed_triangle_area() {
////        return (dot[1].x - dot[0].x) * (dot[2].y - dot[0].y) - (dot[1].y - dot[0].y) * (dot[2].x - dot[0].x);
////    }
////    //差值得到中间像素的uv，借用新的uv读取例如spec文件，diff文件等
////    vec2 uv_gravity(double for_a, double for_b, double for_c)
////    {
////        return uv[2] * for_c + uv[0] * for_a + uv[1] * for_b;
////    }
////    //差值得到中间像素的法线
////    vec4 norm_gravity(double for_a, double for_b, double for_c)
////    {
////        return norm[2] * for_c + norm[0] * for_a + norm[1] * for_b;
////    }
////private:
////
////};
//
////struct BlankShader  {
////    const Model& model;
////
////    BlankShader(const Model& m) : model(m) {}
////
////    TGAColor color(const vec3 bar) const {
////        return {255, 255, 255, 255} ;
////    }
////};
//
////struct PhongShader  {
////    const Model& model;
////    vec4 l;
////
////    PhongShader(const vec3 light, const Model& m) : model(m) {
////        l = normalized((ModelView * vec4{ light.x, light.y, light.z, 0. }));
////    }
////
////    TGAColor color(triangle& tri,const vec3 bar,mat<4,4>modelView_invert_transpose) const {
////
////        mat<2, 4>E = { tri.dot[1] - tri.dot[0],tri.dot[2] - tri.dot[0] };
////        mat<2, 2>U = { tri.uv[1] - tri.uv[0],tri.uv[2] - tri.uv[0] };
////        mat<2, 4>T = U.invert() * E;
////        vec4 t0 = normalized(T[0]);
////        vec4 t1 = normalized(T[1]);
////        mat<4, 4> model_ = ModelView.invert_transpose();
////        vec4 n_t = normalized((model_ * tri.norm_gravity(bar[0], bar[1], bar[2])));
////        mat<4, 4>D = { t0,t1,n_t,{0,0,0,1} };
////
////        vec2 uv = tri.uv_gravity(bar[0], bar[1], bar[2]);
////
////        vec4 n = normalized(D.transpose() * model.normal(uv));
////        vec4 r = normalized(n * (n * l) * 2 - l);
////
////        double ambient = .4;
////        double diff = std::max(0., n * l);
////        double spec = 1.+3. *std::pow(std::max(r.z, 0.), 35) *  static_cast<double>(model.specular(uv)[1]) / 255.0;
////
////        TGAColor gl_FragColor = model.diffuse(uv);
////        double all_light = (ambient + diff + spec);
////        for (int channel : {0, 1, 2})
////            gl_FragColor[channel] = std::min<int>(255, gl_FragColor[channel] * all_light);
////
////        return gl_FragColor ;
////    }
////};
//
////void drop_zbuffer(std::string filename, std::vector<double>& zbuffer, int width, int height) {
////    TGAImage zimg(width, height, TGAImage::GRAYSCALE, { 0,0,0,0 });
////    double minz = +1000;
////    double maxz = -1000;
////    for (int x = 0; x < width; x++) {
////        for (int y = 0; y < height; y++) {
////            double z = zbuffer[x + y * width];
////            if (z < -100) continue;
////            minz = std::min(z, minz);
////            maxz = std::max(z, maxz);
////        }
////    }
////    for (int x = 0; x < width; x++) {
////        for (int y = 0; y < height; y++) {
////            double z = zbuffer[x + y * width];
////            if (z < -100) continue;
////            z = (z - minz) / (maxz - minz) * 255;
////            zimg.set(x, y, { (unsigned char)z, 255, 255, 255 });
////        }
////    }
////    zimg.write_tga_file(filename);
////}
////
////inline int edge(int ax, int ay, int bx, int by, int px, int py) {
////    return (bx - ax) * (py - ay) - (by - ay) * (px - ax);
////}
////void rasterize(const triangle& tri, const PhongShader& shader, TGAImage& framebuffer, std::vector<double>& zbuffer_true,int width,int height) {
////    double space = tri.signed_triangle_area();
////    if (space < 1)
////    {
////        return;
////    }
////    vec4 ndc[3] = { tri.dot[0], tri.dot[1], tri.dot[2]};                // normalized device coordinates
////    vec2 screen[3] = { (Viewport * ndc[0]).xy(), (Viewport * ndc[1]).xy(), (Viewport * ndc[2]).xy() }; // screen coordinates
////
////    int raw_bbminx = std::min(std::min((int)screen[0].x, (int)screen[1].x), (int)screen[2].x);
////    int raw_bbminy = std::min(std::min((int)screen[0].y, (int)screen[1].y), (int)screen[2].y);
////    int raw_bbmaxx = std::max(std::max((int)screen[0].x, (int)screen[1].x), (int)screen[2].x);
////    int raw_bbmaxy = std::max(std::max((int)screen[0].y, (int)screen[1].y), (int)screen[2].y);
////
////    int bbminx = std::max(0, raw_bbminx);
////    int bbminy = std::max(0, raw_bbminy);
////    int bbmaxx = std::min(width - 1, raw_bbmaxx);
////    int bbmaxy = std::min(height - 1, raw_bbmaxy);
////
////    if (bbminx > bbmaxx || bbminy > bbmaxy) {
////        return;
////    }
////    int w = bbmaxx - bbminx + 1;
////    int h = bbmaxy - bbminy + 1;
////
////    int ax = (int)screen[0].x, ay = (int)screen[0].y;
////    int bx = (int)screen[1].x, by = (int)screen[1].y;
////    int cx = (int)screen[2].x, cy = (int)screen[2].y;
////
////    int e0 = edge(ax, ay, bx, by, bbminx, bbminy);
////    int e1 = edge(bx, by, cx, cy, bbminx, bbminy);
////    int e2 = edge(cx, cy, ax, ay, bbminx, bbminy);
////
////    int de0x = -(by - ay);
////    int de1x = -(cy - by);
////    int de2x = -(ay - cy);
////
////    int de0y = (bx - ax);
////    int de1y = (cx - bx);
////    int de2y = (ax - cx);
////    mat<4, 4> model_ = ModelView.invert_transpose();
////#pragma omp parallel for private(ce0, ce1, ce2)
////    for (int j = 0; j < h; j++) {
////        int ce0 = e0 + de0y * j;
////        int ce1 = e1 + de1y * j;
////        int ce2 = e2 + de2y * j;
////        for (int i = 0; i < w; i++) {
////            bool inside =
////                (ce0 >= 0 && ce1 >= 0 && ce2 >= 0) ||
////                (ce0 <= 0 && ce1 <= 0 && ce2 <= 0);
////
////            if (inside) {
////                double for_c = (double)ce0 / space;
////                double for_a = (double)ce1 / space;
////                double for_b = (double)ce2 / space;
////                const vec3 bar = { for_a,for_b ,for_c };
////                TGAColor color_more_real = shader.color(tri,bar,model_);
////                double z = tri.dot[0].z * for_a + tri.dot[1].z * for_b + tri.dot[2].z * for_c;
////
////                int idx = bbminx + i + (bbminy + j) * width;
////                    if (z > zbuffer_true[idx])
////                    {
////                        framebuffer.set(bbminx + i, bbminy + j, color_more_real);
////                        zbuffer_true[idx] = z;
////                    }
////            }
////            ce0 += de0x;
////            ce1 += de1x;
////            ce2 += de2x;
////        }
////    }
////}
////    vec4 ndc[3] = { tri.dot[0], tri.dot[1], tri.dot[2]};                // normalized device coordinates
////    vec2 screen[3] = { (Viewport * ndc[0]).xy(), (Viewport * ndc[1]).xy(), (Viewport * ndc[2]).xy() }; // screen coordinates
////    mat<3, 3> ABC = { { {screen[0].x, screen[0].y, 1.}, {screen[1].x, screen[1].y, 1.}, {screen[2].x, screen[2].y, 1.} } };
////    if (ABC.det() < 1) return; // backface culling + discarding triangles that cover less than a pixel
////
////    //auto [bbminx, bbmaxx] = std::minmax({ screen[0].x, screen[1].x, screen[2].x }); // bounding box for the triangle
////    //auto [bbminy, bbmaxy] = std::minmax({ screen[0].y, screen[1].y, screen[2].y }); // defined by its top left and bottom right corners
////
////    int bbminx = std::min(std::min(screen[0].x, screen[1].x), screen[2].x);
////    int bbminy = std::min(std::min(screen[0].y, screen[1].y), screen[2].y);
////    int bbmaxx = std::max(std::max(screen[0].x, screen[1].x), screen[2].x);
////    int bbmaxy = std::max(std::max(screen[0].y, screen[1].y), screen[2].y);
////
////#pragma omp parallel for
////    for (int x = std::max<int>(bbminx, 0); x <= std::min<int>(bbmaxx, framebuffer.width() - 1); x++) {         // clip the bounding box by the screen
////        for (int y = std::max<int>(bbminy, 0); y <= std::min<int>(bbmaxy, framebuffer.height() - 1); y++) {
////            vec3 bc_screen = ABC.invert_transpose() * vec3 { static_cast<double>(x), static_cast<double>(y), 1. }; // barycentric coordinates of {x,y} w.r.t the triangle
////            vec3 bc_clip = bc_screen;// { bc_screen.x / clip[0].w, bc_screen.y / clip[1].w, bc_screen.z / clip[2].w };     // check https://github.com/ssloy/tinyrenderer/wiki/Technical-difficulties-linear-interpolation-with-perspective-deformations
////            //bc_clip = bc_clip / (bc_clip.x + bc_clip.y + bc_clip.z);
////            if (bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0) continue;                                                    // negative barycentric coordinate => the pixel is outside the triangle
////            double z = bc_screen * vec3{ ndc[0].z, ndc[1].z, ndc[2].z };  // linear interpolation of the depth
////            if (z <= zbuffer[x + y * framebuffer.width()]) continue;   // discard fragments that are too deep w.r.t the z-buffer
////            TGAColor color = shader.fragment(bc_clip);
////            //if (discard) continue;                                 // fragment shader can discard current fragment
////            zbuffer[x + y * framebuffer.width()] = z;                  // update the z-buffer
////            framebuffer.set(x, y, color);                          // update the framebuffer
////        }
////    }