#pragma once

#include <vector>
#include <string>
#include <cstdint>
#include "Color.h"

namespace Render {
    
    typedef Color::RGB Pixel;

    struct RenderBuffer {
        size_t width;
        size_t height;
        std::vector<std::vector<Pixel>> pixelMatrix;
    };

    /** Helper function to calculate RenderBuffer width and height values from pixelMatrix. */
    void CalcSize(RenderBuffer& rb);

    /** Fill the whole RenderBuffer.pixelMatrix with a specific color. */
    void Fill(RenderBuffer& rb, const Pixel pixel);

    /** Generate a map that holds diferent Shades of a given hue. */
    bool GenerateShadeMap(RenderBuffer& rb, float hue);

    /** Generates a map that holds all hues in the RGB color spectrum. */
    bool GenerateHueMap(RenderBuffer& rb);

    /** Converts and outputs a RenderBuffer object into
    a std::string buffer by rendering pixels as ascii characters with RGB ansi color. */
    void RenderANSIString(std::string& buffer, RenderBuffer& rb);

    Pixel GetShadeColor(size_t width, size_t height, float hue, size_t x, size_t y);
}