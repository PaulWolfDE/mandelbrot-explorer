#ifndef COLOR_H
#define COLOR_H

const float third = 1.0f / 3.0f;

class Color {

public:
    Color(int r, int g, int b);
    Color(float r, float g, float b);
    Color(int h, float s, float l);

    int r() const;
    int g() const;
    int b() const;

    int argb() const;

private:
    int r_, g_, b_;
    int a_ = 255;
};

#endif //COLOR_H
