#include "PPM.h"
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <tuple> 

void PPM::CalcImageSize(Image& image) {
    image.height = image.pixelMatrix.size();
    image.width = image.height > 0 ? image.pixelMatrix[0].size() : 0;
}

std::string PPM::PixelToANSI(PPM::Pixel& p, bool fg) {
    if (fg) {
        return "\033[38;2;" + std::to_string(p.r) + ";" + std::to_string(p.g) + ";" + std::to_string(p.b) + "m";
    } else {
        return "\033[48;2;" + std::to_string(p.r) + ";" + std::to_string(p.g) + ";" + std::to_string(p.b) + "m";
    }
}

bool PPM::Read(const std::string& filename, PPM::Image& image) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Cannot open file\n";
        return false;
    }

    std::string line, magic;
    // Read magic number (should be P6)
    getline(file, magic);
    if (magic != "P6") {
        std::cerr << "Not a P6 PPM file\n";
        return false;
    }

    // Skip comments and read width & height
    do {
        getline(file, line);
    } while (line[0] == '#');

    std::istringstream dimensions(line);
    dimensions >> image.width >> image.height;

    // Read max color value (usually 255)
    int max_val;
    file >> max_val;
    file.ignore(1); // skip single whitespace after max_val

    if (max_val != 255) {
        std::cerr << "Only max color 255 supported\n";
        return false;
    }

    // Resize image container
    image.pixelMatrix.resize(image.height, std::vector<PPM::Pixel>(image.width));

    // Read raw pixel data
    std::vector<uint8_t> buffer(image.width * 3);

    for (size_t y = 0; y < image.height; ++y) {
        file.read(reinterpret_cast<char*>(buffer.data()), image.width * 3);
        if (!file) {
            std::cerr << "Error reading pixel data\n";
            return false;
        }
        for (size_t x = 0; x < image.width; ++x) {
            image.pixelMatrix[y][x].r = buffer[3 * x];
            image.pixelMatrix[y][x].g = buffer[3 * x + 1];
            image.pixelMatrix[y][x].b = buffer[3 * x + 2];
        }
    }
    

    return true;
}

bool PPM::Write(const std::string& filename, PPM::Image& image) {
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Cannot open file\n";
        return false;
    }

    PPM::CalcImageSize(image);

    std::string header = "P6\n" + std::to_string(image.width) + " " + std::to_string(image.height) + "\n255\n";
    file.write(header.data(), header.size());

    // Generate pixel data
    std::vector<uint8_t> buffer(image.width * 3);

    for (size_t y = 0; y < image.height; ++y) {
        // Fill the buffer from the image row
        for (size_t x = 0; x < image.width; ++x) {
            buffer[3 * x]     = image.pixelMatrix[y][x].r;
            buffer[3 * x + 1] = image.pixelMatrix[y][x].g;
            buffer[3 * x + 2] = image.pixelMatrix[y][x].b;
        }

        // Write the row to the file
        file.write(reinterpret_cast<char*>(buffer.data()), image.width * 3);
        if (!file) {
            std::cerr << "Error writing pixel data\n";
            return false;
        }
    }

    return true;
}

void PPM::ToString(std::string& buffer, PPM::Image& image) {
    size_t line = 0;
    while (line < image.height) {
        for (size_t col = 0; col < image.width; col++) {
            PPM::Pixel pixel = image.pixelMatrix[line][col];
            if (line + 1 < image.height) {
                PPM::Pixel pixelNextLine = image.pixelMatrix[line + 1][col];
                if (pixel == pixelNextLine) {
                    buffer += PPM::PixelToANSI(pixel, true) + "█\033[0m";
                } else {
                    buffer += PPM::PixelToANSI(pixel, true) + PPM::PixelToANSI(pixelNextLine, false) + "▀\033[0m";
                }
            } else {
                buffer += PPM::PixelToANSI(pixel, true) + "▀\033[0m";
            }
        }
        buffer += "\n";
        line += 2;
    }
}

void PPM::Fill(PPM::Image& image, const PPM::Pixel pixel) {
    std::vector<Pixel> line;
    for (size_t w = 0; w < image.width; w++) line.push_back(pixel);
    for (size_t h = 0; h < image.height; h++) image.pixelMatrix.push_back(line);
}

PPM::Pixel PPM::HSLtoRGB(float h, float s, float l) {
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
        static_cast<uint8_t>(std::clamp(r * 255.0f, 0.0f, 255.0f)),
        static_cast<uint8_t>(std::clamp(g * 255.0f, 0.0f, 255.0f)),
        static_cast<uint8_t>(std::clamp(b * 255.0f, 0.0f, 255.0f))
    };
}

PPM::HSL PPM::RGBtoHSL(const PPM::Pixel& px) {
        // normalize to [0,1]
        float r = px.r / 255.0f;
        float g = px.g / 255.0f;
        float b = px.b / 255.0f;

        float maxVal = std::max({r, g, b});
        float minVal = std::min({r, g, b});
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

bool PPM::GenerateShadeMap(PPM::Image& image, float hue) {
    if (image.width == 0 || image.height == 0) return false;

    image.pixelMatrix.resize(image.height, std::vector<PPM::Pixel>(image.width));

    for (size_t y = 0; y < image.height; ++y) {
        for (size_t x = 0; x < image.width; ++x) {
            float saturation = static_cast<float>(x) / (image.width - 1);
            float lightness = 1.0f - static_cast<float>(y) / (image.height - 1);
            image.pixelMatrix[y][x] = HSLtoRGB(hue, saturation, lightness);
        }
    }

    return true;
}

bool PPM::GenerateHueMap(PPM::Image& image) {
    if (image.width == 0 || image.height == 0) return false;

    image.pixelMatrix.resize(image.height, std::vector<PPM::Pixel>(image.width));

    const float max_hue = 1.0f;
    const float increments = max_hue / image.height;

    float hue = 0.0f;
    for (size_t y = 0; y < image.height; ++y, hue += increments) {
        PPM::Pixel hue_pixel = HSLtoRGB(hue, 1.f, 0.5f); // l=0.5 for vivid hue
        for (size_t x = 0; x < image.width; ++x) {
            image.pixelMatrix[y][x] = hue_pixel;
        }
    }

    return true;
}