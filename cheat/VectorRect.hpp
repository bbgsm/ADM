#pragma once

struct VectorRect {
    float x;
    float y;
    float w;
    float h;

    VectorRect():
        x(0), y(0), w(0), h(0) {
    }

    VectorRect(float _x, float _y, float _w, float _h) {
        x = _x;
        y = _y;
        w = _w;
        h = _h;
    }
};
