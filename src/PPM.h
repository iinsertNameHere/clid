#pragma once

#include <vector>
#include <string>
#include <cstdint>

namespace PPM {
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

    struct Image {
        size_t width;
        size_t height;
        std::vector<std::vector<Pixel>> pixelMatrix;
    };

    /** Helper function to calculate size values from pixelMatrix. */
    void CalcImageSize(Image& image);

    /** Fill the whole pixelMatrix with a specific color. */
    void Fill(Image& image, const Pixel pixel);

    /** Convert HSL color to an RGB Pixel. */
    Pixel HSLtoRGB(float h, float s, float l);

    /** Convert PPM::Pixel to HSL values. */
    HSL RGBtoHSL(const Pixel& px);

    /** Generate a map that holds diferent Shades of a given hue. */
    bool GenerateShadeMap(Image& image, float hue);

    /** Generates a map that holds all hues in the RGB color spectrum. */
    bool GenerateHueMap(Image& image);

    /** Converts a PPM::Pixel to an ansi color code. */
    std::string PixelToANSI(Pixel& p, bool fg = true);

    /** Reads a .ppm file and populates the provided PPM::Image object with its data. */
    bool Read(const std::string& filename, Image& image);

    /** Writes a PPM::Image object to a .ppm file */
    bool Write(const std::string& filename, Image& image);

    /** Converts and outputs a PPM::Image object into
    a std::string buffer by rendering pixels as ascii characters with RGB ansi color. */
    void ToString(std::string& buffer, Image& image);

}