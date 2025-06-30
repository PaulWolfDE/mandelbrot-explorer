#include "color.h"

Color::Color(int r, int g, int b) : r_(r), g_(g), b_(b) {}

Color::Color(float r, float g, float b) : r_(r*255), g_(g*255), b_(b*255) {}

// Implementation inspired by: https://www.niwa.nu/2013/05/math-behind-colorspace-conversions-rgb-hsl/
Color::Color(int h, float s, float l)
{
    if (s == 0.0f) {
        r_ = l*255;
        g_ = l*255;
        b_ = l*255;
        return;
    }

    float t1 = l < 0.5f ? l*(1+s) : l+s-l*s;
    float t2 = 2.0f*l-t1;

    float relH = h/360.0f;

    float tr = relH + third, tg = relH, tb = relH - third;

    if (tr > 1.0f) tr -= 1.0f;
    if (tr < 0.0f) tr += 1.0f;
    if (tg > 1.0f) tg -= 1.0f;
    if (tg < 0.0f) tg += 1.0f;
    if (tb > 1.0f) tb -= 1.0f;
    if (tb < 0.0f) tb += 1.0f;

    auto color_chanel = [](float t, float t1, float t2, float third){

        if (6.0f*t < 1) return t2 + (t1-t2) * 6.0f * t;
        if (2.0f*t < 1) return t1;
        if (3.0f*t < 2) return t2 + (t1-t2) * (2.0f*third - t) * 6.0f;
        return t2;
    };

    r_ = color_chanel(tr, t1, t2, third)*255;
    g_ = color_chanel(tg, t1, t2, third)*255;
    b_ = color_chanel(tb, t1, t2, third)*255;
}


int Color::r() const { return r_; }
int Color::g() const { return g_; }
int Color::b() const { return b_; }

int Color::argb() const
{
    return (a_ << 24) | (r_ << 16) | (g_ << 8) | b_;
}

