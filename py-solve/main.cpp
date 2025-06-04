#define STB_IMAGE_IMPLEMENTATION
#include "deps/stb_image.h"
#include <iostream>
#include <vector>
#include <chrono>
#include <cmath>
#include <tuple>

using namespace std;
using Clock = chrono::high_resolution_clock;

struct Timer {
    chrono::time_point<Clock> start, last;
    Timer() : start(Clock::now()), last(start) {}

    void step(const string& label) {
        auto now = Clock::now();
        chrono::duration<double> dur = now - last;
        cout << "[" << label << "] Step took: " << dur.count() << " seconds\n";
        last = now;
    }

    void total() {
        chrono::duration<double> dur = Clock::now() - start;
        cout << "Total time: " << dur.count() << " seconds\n";
    }
};

struct Img {
    int w, h;
    vector<uint8_t> mtx;  // 1s and 0s

    Img(int w_, int h_, const vector<uint8_t>& pxs) : w(w_), h(h_), mtx(pxs) {}

    uint8_t getpx(int x, int y) const {
        return mtx[x + y * w];
    }
};

struct Shape {
    int id;
    vector<pair<int, int>> pixels;

    explicit Shape(int id_) : id(id_) {}
    void addpx(int x, int y) { pixels.emplace_back(x, y); }
    bool has() const { return !pixels.empty(); }
};

struct MaskMatrix {
    int w, h;
    vector<int> mtx;

    MaskMatrix(int w_, int h_) : w(w_), h(h_), mtx(w_ * h_, 0) {}

    int getat(int x, int y) const { return mtx[x + y * w]; }
    void setat(int x, int y, int val) { mtx[x + y * w] = val; }
};

const vector<pair<int, int>> OFFSETS = {
    {0, -1}, {1, 0}, {0, 1}, {-1, 0}
};

void dfs(const Img& img, int start_x, int start_y, Shape& shape, MaskMatrix& mask) {
    vector<pair<int, int>> stack = {{start_x, start_y}};
    mask.setat(start_x, start_y, shape.id);

    while (!stack.empty()) {
        auto [x, y] = stack.back();
        stack.pop_back();
        shape.addpx(x, y);

        for (const auto& [dx, dy] : OFFSETS) {
            int nx = x + dx;
            int ny = y + dy;
            if (nx < 0 || ny < 0 || nx >= img.w || ny >= img.h) continue;
            if (img.getpx(nx, ny) != 1) continue;
            if (mask.getat(nx, ny) != 0) continue;
            mask.setat(nx, ny, shape.id);
            stack.emplace_back(nx, ny);
        }
    }
}

struct BoundingRect {
    int min_x = INT_MAX, min_y = INT_MAX;
    int max_x = 0, max_y = 0;
    pair<float, float> centroid;
};

BoundingRect get_bounding_rect(const Shape& s) {
    BoundingRect br;
    int totalx = 0, totaly = 0;
    for (auto [x, y] : s.pixels) {
        totalx += x;
        totaly += y;
        br.max_x = max(br.max_x, x);
        br.max_y = max(br.max_y, y);
        br.min_x = min(br.min_x, x);
        br.min_y = min(br.min_y, y);
    }
    br.centroid = {
        totalx / static_cast<float>(s.pixels.size()),
        totaly / static_cast<float>(s.pixels.size())
    };
    return br;
}

float get_white_area_percentage(const BoundingRect& rect, const Img& img) {
    int w = rect.max_x - rect.min_x;
    int h = rect.max_y - rect.min_y;
    int totalpx = w * h;
    int whitepx = 0;
    for (int x = 0; x < w; ++x) {
        for (int y = 0; y < h; ++y) {
            if (img.getpx(rect.min_x + x, rect.min_y + y))
                ++whitepx;
        }
    }
    return static_cast<float>(whitepx) / totalpx;
}

int main() {
    Timer timer;

    int width, height, channels;
    // unsigned char* data = stbi_load("4.png", &width, &height, &channels, 3);
    unsigned char* data = stbi_load("4.bmp", &width, &height, &channels, 3);
    if (!data) {
        cerr << "Failed to load image.\n";
        return 1;
    }

    cout << "Opened image [" << width << " x " << height << "]\n";
    timer.step("Open image");

    const int THRESHOLD = 254;
    vector<uint8_t> binmtx;
    binmtx.reserve(width * height);

    for (int y = 0; y < height; ++y)
        for (int x = 0; x < width; ++x) {
            int i = (x + y * width) * 3;
            binmtx.push_back(data[i + 2] > THRESHOLD ? 1 : 0);
        }

    stbi_image_free(data);
    timer.step("Apply Threshold");

    Img pimg(width, height, binmtx);
    MaskMatrix mask(width, height);
    vector<Shape> shapes;

    int id = 1;
    for (int y = 0; y < height; ++y)
        for (int x = 0; x < width; ++x)
            if (pimg.getpx(x, y) == 1 && mask.getat(x, y) == 0) {
                Shape shape(id);
                dfs(pimg, x, y, shape, mask);
                if (shape.has())
                    shapes.push_back(shape);
                id++;
            }

    timer.step("Detect shapes");

    // Filter
    vector<Shape> filtered;
    for (const auto& s : shapes)
        if (s.pixels.size() > 20)
            filtered.push_back(s);

    shapes = move(filtered);
    timer.step("Filter shapes");

    const int TOLERANCE = 5;
    int redondos = 0, pilulas = 0, quebrados = 0;

    for (const auto& shape : shapes) {
        BoundingRect br = get_bounding_rect(shape);
        int dx = br.max_x - br.min_x;
        int dy = br.max_y - br.min_y;

        float p = get_white_area_percentage(br, pimg);
        // cout << "percentarea == " << p << "\n";

        float LIM_PILULA_INF = 0.6f;
        float LIM_PILULA_SUP = 0.9f;

        if (abs(dx - dy) > TOLERANCE) {
            if (p > LIM_PILULA_INF && p <= LIM_PILULA_SUP)
                quebrados++;
            else
                pilulas++;
        } else {
            redondos++;
        }
    }

    timer.step("Do math");

    cout << shapes.size() << " Comprimidos estão na esteira\n";
    cout << pilulas << " Pilulas\n";
    cout << redondos << " Redondos\n";
    cout << quebrados << " Quebrados\n";

    timer.total();
    return 0;
}
