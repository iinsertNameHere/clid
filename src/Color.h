#pragma once

#include <cstdint>
#include <string>

namespace Color {

    struct RGB {
        uint8_t r;
        uint8_t g;
        uint8_t b;
    };

    struct CMYK {
        float c;
        float m;
        float y;
        float k;
    };

    struct HSL {
        float h;
        float s;
        float l;
    };

    typedef std::string HEX;
    typedef std::string ANSI;

    void RGBtoHSL(HSL& out, const RGB& in);
    void HSLtoRGB(RGB& out, const HSL& in);
    void RGBtoHEX(HEX& out, const RGB& in);
    bool HEXtoRGB(RGB& out, const HEX& in);
    void RGBtoCMYK(CMYK& out, const RGB& in);
    void CMYKtoRGB(RGB& out, const CMYK& in);

    ANSI RGBtoANSI(const RGB& in, bool fg = true);
}