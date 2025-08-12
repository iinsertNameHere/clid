#pragma once

#include <vector>
#include <string>
#include <cstdint>

namespace Render {
    struct Pixel {
        uint8_t r;
        uint8_t g;
        uint8_t b;

        bool operator==(const Pixel& p) const {
            return r == p.r && g == p.g && b == p.b;
        }
    };

    struct HSL {
        float h;
        float s;
        float l;
    };

    struct RenderBuffer {
        size_t width;
        size_t height;
        std::vector<std::vector<Pixel>> pixelMatrix;
    };

    /** Helper function to calculate RenderBuffer width and height values from pixelMatrix. */
    void CalcSize(RenderBuffer& rb);

    /** Fill the whole RenderBuffer.pixelMatrix with a specific color. */
    void Fill(RenderBuffer& rb, const Pixel pixel);

    /** Convert HSL color to an RGB Pixel. */
    Pixel HSLtoRGB(float h, float s, float l);

    /** Convert Pixel to HSL values. */
    HSL RGBtoHSL(const Pixel& px);

    /** Generate a map that holds diferent Shades of a given hue. */
    bool GenerateShadeMap(RenderBuffer& rb, float hue);

    /** Generates a map that holds all hues in the RGB color spectrum. */
    bool GenerateHueMap(RenderBuffer& rb);

    /** Converts a Pixel to an ansi color code. */
    std::string PixelToANSI(Pixel& p, bool fg = true);

    /** Converts and outputs a RenderBuffer object into
    a std::string buffer by rendering pixels as ascii characters with RGB ansi color. */
    void RenderANSIString(std::string& buffer, RenderBuffer& rb);

}