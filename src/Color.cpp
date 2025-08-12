#include "Color.h"
#include <cmath>
#include <algorithm>
#include <sstream>
#include <iomanip>

void Color::RGBtoHSL(HSL& out, const RGB& in) {
    float rf = in.r / 255.0f;
    float gf = in.g / 255.0f;
    float bf = in.b / 255.0f;

    float max = std::max({rf, gf, bf});
    float min = std::min({rf, gf, bf});
    float delta = max - min;

    out.h = 0.0f;
    if (delta != 0.0f) {
        if (max == rf) {
            out.h = fmod((gf - bf) / delta, 6.0f);
            if (out.h < 0.0f) out.h += 6.0f;
        } else if (max == gf) {
            out.h = ((bf - rf) / delta) + 2.0f;
        } else {
            out.h = ((rf - gf) / delta) + 4.0f;
        }
        out.h /= 6.0f;  // Convert to [0.0, 1.0]
    }

    out.l = (max + min) / 2.0f;

    out.s = 0.0f;
    if (delta != 0.0f) {
        out.s = delta / (1.0f - std::fabs(2.0f * out.l - 1.0f));
    }
}

void Color::HSLtoRGB(RGB& out, const HSL& in) {
    float h = in.h * 6.0f;  // Convert back to [0, 6) sector space

    float c = (1.0f - std::fabs(2.0f * in.l - 1.0f)) * in.s;
    float x = c * (1.0f - std::fabs(fmod(h, 2.0f) - 1.0f));
    float m = in.l - c / 2.0f;

    float rf = 0.0f, gf = 0.0f, bf = 0.0f;

    if (h < 1.0f) {
        rf = c; gf = x; bf = 0;
    } else if (h < 2.0f) {
        rf = x; gf = c; bf = 0;
    } else if (h < 3.0f) {
        rf = 0; gf = c; bf = x;
    } else if (h < 4.0f) {
        rf = 0; gf = x; bf = c;
    } else if (h < 5.0f) {
        rf = x; gf = 0; bf = c;
    } else {
        rf = c; gf = 0; bf = x;
    }

    out.r = static_cast<int>((rf + m) * 255.0f + 0.5f);
    out.g = static_cast<int>((gf + m) * 255.0f + 0.5f);
    out.b = static_cast<int>((bf + m) * 255.0f + 0.5f);
}


void Color::RGBtoHEX(HEX& out, const RGB& in) {
    std::ostringstream oss;
    oss << '#' 
        << std::hex << std::uppercase << std::setfill('0') 
        << std::setw(2) << static_cast<int>(in.r) 
        << std::setw(2) << static_cast<int>(in.g) 
        << std::setw(2) << static_cast<int>(in.b);
    out = oss.str();
}

bool Color::HEXtoRGB(RGB& out, const HEX& in) {
    HEX cleanHex = in;
    if (cleanHex.size() == 7 && cleanHex[0] == '#') {
        cleanHex = cleanHex.substr(1);
    } else if (cleanHex.size() != 6) {
        return false;
    }

    try {
        out.r = static_cast<uint8_t>(std::stoi(cleanHex.substr(0, 2), nullptr, 16));
        out.g = static_cast<uint8_t>(std::stoi(cleanHex.substr(2, 2), nullptr, 16));
        out.b = static_cast<uint8_t>(std::stoi(cleanHex.substr(4, 2), nullptr, 16));
    } catch (...) {
        return false;
    }

    return true;
}

void Color::RGBtoCMYK(CMYK& out, const RGB& in) {
    float rf = in.r / 255.0f;
    float gf = in.g / 255.0f;
    float bf = in.b / 255.0f;

    out.k = 1.0f - std::max({rf, gf, bf});

    if (out.k == 1.0f) {
        out = {0, 0, 0, 1}; // Pure black
        return;
    }

    out.c = (1 - rf - out.k) / (1 - out.k);
    out.m = (1 - gf - out.k) / (1 - out.k);
    out.y = (1 - bf - out.k) / (1 - out.k);
}

void Color::CMYKtoRGB(RGB& out, const CMYK& in) {
    out.r = static_cast<int>(255 * (1 - in.c) * (1 - in.k));
    out.g = static_cast<int>(255 * (1 - in.m) * (1 - in.k));
    out.b = static_cast<int>(255 * (1 - in.y) * (1 - in.k));
}

Color::ANSI Color::RGBtoANSI(const RGB& in, bool fg) {
    if (fg) {
        return "\033[38;2;" + std::to_string(in.r) + ";" + std::to_string(in.g) + ";" + std::to_string(in.b) + "m";
    } else {
        return "\033[48;2;" + std::to_string(in.r) + ";" + std::to_string(in.g) + ";" + std::to_string(in.b) + "m";
    }
}