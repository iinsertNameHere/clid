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

void Render::Fill(RenderBuffer& rb, const Pixel pixel) {
    vector<Pixel> line;
    for (size_t w = 0; w < rb.width; w++) line.push_back(pixel);
    for (size_t h = 0; h < rb.height; h++) rb.pixelMatrix.push_back(line);
}

bool Render::GenerateShadeMap(RenderBuffer& rb, float hue) {
    if (rb.width == 0 || rb.height == 0) return false;

    rb.pixelMatrix.resize(rb.height, vector<Pixel>(rb.width));

    for (size_t y = 0; y < rb.height; ++y) {
        for (size_t x = 0; x < rb.width; ++x) {
            float saturation = static_cast<float>(x) / (rb.width - 1);
            float lightness = 1.0f - static_cast<float>(y) / (rb.height - 1);
            Color::RGB rgb;
            Color::HSLtoRGB(rgb, {hue, saturation, lightness});
            rb.pixelMatrix[y][x] = rgb;
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
        Color::RGB hue_rgb;
        Color::HSLtoRGB(hue_rgb, {hue, 1.f, 0.5f}); // l=0.5 for vivid hue
        for (size_t x = 0; x < rb.width; ++x) {
            rb.pixelMatrix[y][x] = hue_rgb;
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
                if (pixel.r == pixelNextLine.r && pixel.g == pixelNextLine.g && pixel.b == pixelNextLine.b) {
                    buffer += Color::RGBtoANSI(pixel, true) + "█\033[0m";
                } else {
                    buffer += Color::RGBtoANSI(pixel, true) + Color::RGBtoANSI(pixelNextLine, false) + "▀\033[0m";
                }
            } else {
                buffer += Color::RGBtoANSI(pixel, true) + "▀\033[0m";
            }
        }
        buffer += "\n";
        line += 2;
    }
}

Render::Pixel Render::GetShadeColor(size_t width, size_t height, float hue, size_t x, size_t y) {
    float lightness = 1.0f - (float)y / (height - 1); // top is bright, bottom is dark
    float saturation = (float)x / (width - 1);        // left is gray, right is full color
    
    Color::RGB rgb;
    Color::HSLtoRGB(rgb, {hue, saturation, lightness});
    return rgb;
}