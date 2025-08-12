#include "Render.h"
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <tuple> 

using namespace Render;
using namespace std;

void Render::CalcSize(RenderBuffer& rb) {
    rb.height = rb.pixelMatrix.size();
    rb.width = rb.height > 0 ? rb.pixelMatrix[0].size() : 0;
}

string Render::PixelToANSI(Pixel& p, bool fg) {
    if (fg) {
        return "\033[38;2;" + to_string(p.r) + ";" + to_string(p.g) + ";" + to_string(p.b) + "m";
    } else {
        return "\033[48;2;" + to_string(p.r) + ";" + to_string(p.g) + ";" + to_string(p.b) + "m";
    }
}

void Render::Fill(RenderBuffer& rb, const Pixel pixel) {
    vector<Pixel> line;
    for (size_t w = 0; w < rb.width; w++) line.push_back(pixel);
    for (size_t h = 0; h < rb.height; h++) rb.pixelMatrix.push_back(line);
}

Pixel Render::HSLtoRGB(float h, float s, float l) {
    auto hue2rgb = [](float p, float q, float t) {
        if (t < 0) t += 1;
        if (t > 1) t -= 1;
        if (t < 1.0f/6) return p + (q - p) * 6 * t;
        if (t < 1.0f/2) return q;
        if (t < 2.0f/3) return p + (q - p) * (2.0f/3 - t) * 6;
        return p;
    };

    float r, g, b;

    if (s == 0) {
        r = g = b = l; // grayscale
    } else {
        float q = l < 0.5 ? l * (1 + s) : l + s - l * s;
        float p = 2 * l - q;
        r = hue2rgb(p, q, h + 1.0f/3);
        g = hue2rgb(p, q, h);
        b = hue2rgb(p, q, h - 1.0f/3);
    }

    return {
        static_cast<uint8_t>(clamp(r * 255.0f, 0.0f, 255.0f)),
        static_cast<uint8_t>(clamp(g * 255.0f, 0.0f, 255.0f)),
        static_cast<uint8_t>(clamp(b * 255.0f, 0.0f, 255.0f))
    };
}

HSL Render::RGBtoHSL(const Pixel& px) {
        // normalize to [0,1]
        float r = px.r / 255.0f;
        float g = px.g / 255.0f;
        float b = px.b / 255.0f;

        float maxVal = max({r, g, b});
        float minVal = min({r, g, b});
        float h, s, l;

        l = (maxVal + minVal) / 2.0f;

        if (maxVal == minVal) {
            // achromatic (gray)
            h = 0.0f;
            s = 0.0f;
        } else {
            float d = maxVal - minVal;
            s = (l > 0.5f) ? d / (2.0f - maxVal - minVal) : d / (maxVal + minVal);

            if (maxVal == r) {
                h = (g - b) / d + (g < b ? 6.0f : 0.0f);
            } else if (maxVal == g) {
                h = (b - r) / d + 2.0f;
            } else {
                h = (r - g) / d + 4.0f;
            }

            h /= 6.0f; // normalize hue to [0,1]
        }

        return {h, s, l};
    }

bool Render::GenerateShadeMap(RenderBuffer& rb, float hue) {
    if (rb.width == 0 || rb.height == 0) return false;

    rb.pixelMatrix.resize(rb.height, vector<Pixel>(rb.width));

    for (size_t y = 0; y < rb.height; ++y) {
        for (size_t x = 0; x < rb.width; ++x) {
            float saturation = static_cast<float>(x) / (rb.width - 1);
            float lightness = 1.0f - static_cast<float>(y) / (rb.height - 1);
            rb.pixelMatrix[y][x] = HSLtoRGB(hue, saturation, lightness);
        }
    }

    return true;
}

bool Render::GenerateHueMap(RenderBuffer& rb) {
    if (rb.width == 0 || rb.height == 0) return false;

    rb.pixelMatrix.resize(rb.height, vector<Pixel>(rb.width));

    const float increments = 1.0f / rb.height;

    float hue = 0.0f;
    for (size_t y = 0; y < rb.height; ++y, hue += increments) {
        Pixel hue_pixel = HSLtoRGB(hue, 1.f, 0.5f); // l=0.5 for vivid hue
        for (size_t x = 0; x < rb.width; ++x) {
            rb.pixelMatrix[y][x] = hue_pixel;
        }
    }

    return true;
}

void Render::RenderANSIString(string& buffer, RenderBuffer& rb) {
    size_t line = 0;
    while (line < rb.height) {
        for (size_t col = 0; col < rb.width; col++) {
            Pixel pixel = rb.pixelMatrix[line][col];
            if (line + 1 < rb.height) {
                Pixel pixelNextLine = rb.pixelMatrix[line + 1][col];
                if (pixel == pixelNextLine) {
                    buffer += PixelToANSI(pixel, true) + "█\033[0m";
                } else {
                    buffer += PixelToANSI(pixel, true) + PixelToANSI(pixelNextLine, false) + "▀\033[0m";
                }
            } else {
                buffer += PixelToANSI(pixel, true) + "▀\033[0m";
            }
        }
        buffer += "\n";
        line += 2;
    }
}