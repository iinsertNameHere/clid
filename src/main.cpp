#include "Render.h"
#include "Input.h"
#include "Utility.h"
#include "Color.h"

#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <iomanip>

#define VERSION "v1.0.0"

enum class Format { RGB, HEX, CMYK, HSL };

struct AppState {
    Format format = Format::RGB;
    int xSize = 25;
    int ySize = 25;
    Color::HSL hue = {0.0f, 1.0f, 1.0f};
    int selectedX = 0;
    int selectedY = 0;
    bool running = true;
    bool wipeScreen = true;
};
AppState state;

// -------------------------------------------------------------
// CLI HELPERS
// -------------------------------------------------------------
void printUsage() {
    std::cout <<
        "Usage: clid [ARGUMENTS]\n\n"
        "ARGUMENTS:\n"
        "    -h, --help          Show this help list.\n"
        "    -V, --version       Show version number\n"
        "    -v, --view={color}  Preview a color. (Set format using '--format')\n"
        "    -s, --size={num}    Number of pixels for width and height.\n"
        "    -f, --format={str}  Set output format. (Default: 'rgb')\n"
        "    -W, --no-wipe       Leave color picker displayed at exit\n"
        "\n"
        "Example runs:\n"
        "  Run clid in normal mode; choose a color and receive it on stdout on quit\n"
        "    $ clid\n"
        "  Same as before but output in cmyk format\n"
        "    $ clid --format=cmyk\n"
        "  Pipe the output of clid into the clipboard\n"
        "    $ ./clid --format=hex | wl-copy            # For wayland\n"
        "    $ ./clid --format=hex | xsel -i -b         # For X11\n"
        "    $ ./clid --format=hex | xclip -i -sel clip # For X11\n"
        "  Capture output into a variable\n"
        "    $ color=$(./clid)     # Can add options like (./clid -W)\n"
        "\n"
        "OUTPUT FORMATS: rgb, hex, cmyk, hsl\n"
        "\n"
        "TUI CONTROLS:\n"
        "    j  Move hue selector up.\n"
        "    k  Move hue selector down.\n"
        "    w  Move shade selector up.\n"
        "    a  Move shade selector left.\n"
        "    s  Move shade selector down.\n"
        "    d  Move shade selector right.\n"
        "    q  Exit\n";
}

bool parseFormat(const std::string& str, Format& out) {
    if (str == "rgb") out = Format::RGB;
    else if (str == "hex") out = Format::HEX;
    else if (str == "cmyk") out = Format::CMYK;
    else if (str == "hsl") out = Format::HSL;
    else return false;
    return true;
}

// -------------------------------------------------------------
// COLOR INFO GENERATOR
// -------------------------------------------------------------
std::string makeColorInfo(const Color::RGB& rgb) {
    std::ostringstream oss;

    // RGB
    oss << "RGB: " << (int)rgb.r << " " << (int)rgb.g << " " << (int)rgb.b << "\n";

    // HEX
    Color::HEX hexColor;
    Color::RGBtoHEX(hexColor, rgb);
    oss << "HEX: " << hexColor << "\n";

    // HSL
    Color::HSL hsl;
    Color::RGBtoHSL(hsl, rgb);
    oss << std::fixed << std::setprecision(2);
    oss << "HSL: " << hsl.h * 100.0f << " " << hsl.s * 100.0f << " " << hsl.l * 100.0f << "\n";

    // CMYK
    Color::CMYK cmyk;
    Color::RGBtoCMYK(cmyk, rgb);
    oss << "CMYK: " << cmyk.c * 100.0f << " " << cmyk.m * 100.0f << " " << cmyk.y * 100.0f << " " << cmyk.k;

    return oss.str();
}

// -------------------------------------------------------------
// VIEW MODE
// -------------------------------------------------------------
void viewColor(const Color::RGB& color) {
    Render::RenderBuffer colorView; // 8px height
    colorView.width = 4;
    colorView.height = 8;

    Render::Fill(colorView, color);

    std::string colorViewStr;
    Render::RenderANSIString(colorViewStr, colorView);

    std::string info = makeColorInfo(color);

    std::string out;
    Utility::ZipStrings(out, colorViewStr, info);
    std::cerr << out << "\n";
}

// -------------------------------------------------------------
// INPUT HANDLER
// -------------------------------------------------------------
void handleInput(char& key) {
    switch (key) {
        case 'k': state.hue.h = std::min(1.0f, state.hue.h + 0.01f); break;
        case 'j': state.hue.h = std::max(0.0f, state.hue.h - 0.01f); break;
        case 'w': state.selectedY = std::max(0, state.selectedY - 1); break;
        case 'a': state.selectedX = std::max(0, state.selectedX - 1); break;
        case 's': state.selectedY = std::min(state.ySize - 1, state.selectedY + 1); break;
        case 'd': state.selectedX = std::min(state.xSize - 1, state.selectedX + 1); break;
        case 'q':
        case '\n':
            state.running = false; 
            break;
    }
}

// -------------------------------------------------------------
// DRAW LOOP
// -------------------------------------------------------------
std::string drawUI() {
    Render::RenderBuffer shademap;
    shademap.width = state.xSize;
    shademap.height = state.ySize;

    Render::RenderBuffer huemap;
    huemap.width = 4;
    huemap.height = state.ySize;

    Render::RenderBuffer colordisplay;
    colordisplay.width = 4;
    colordisplay.height = 8;

    Render::GenerateShadeMap(shademap, state.hue.h);
    Render::GenerateHueMap(huemap);

    Color::RGB selectedColor = shademap.pixelMatrix[state.selectedY][state.selectedX];
    Render::Fill(colordisplay, selectedColor);

    // Highlight hue
    size_t l = 0;
    bool highlighted = false;
    while (l < huemap.height - 1) {
        Color::HSL h1, h2;
        Color::RGBtoHSL(h1, huemap.pixelMatrix[l][0]);
        Color::RGBtoHSL(h2, huemap.pixelMatrix[l + 1][0]);
        if ((state.hue.h >= h1.h && state.hue.h <= h2.h) ||
            (state.hue.h >= h2.h && state.hue.h <= h1.h)) {
            std::vector<Render::Pixel> row(huemap.width);
            for (auto& px : row) Color::HSLtoRGB(px, {h1.h, 0.4f, 0.4f});
            huemap.pixelMatrix[l] = row;
            highlighted = true;
            break;
        }
        ++l;
    }
    if (!highlighted) {
        Color::HSL lastHue;
        Color::RGBtoHSL(lastHue, huemap.pixelMatrix.back()[0]);
        std::vector<Render::Pixel> row(huemap.width);
        for (auto& px : row) Color::HSLtoRGB(px, {lastHue.h, 0.3f, 0.3f});
        huemap.pixelMatrix.back() = row;
    }

    // Highlight selected shade
    {
        Color::RGB current = shademap.pixelMatrix[state.selectedY][state.selectedX];

        Color::RGB inverted = {
            static_cast<uint8_t>(255 - current.r),
            static_cast<uint8_t>(255 - current.g),
            static_cast<uint8_t>(255 - current.b)
        };

        shademap.pixelMatrix[state.selectedY][state.selectedX] = inverted;
    }

    // Convert to strings
    std::string shademapStr, huemapStr, colordisplayStr;
    Render::RenderANSIString(shademapStr, shademap);
    Render::RenderANSIString(huemapStr, huemap);
    Render::RenderANSIString(colordisplayStr, colordisplay);

    std::string info = makeColorInfo(selectedColor);

    std::string colorInfoBuffer;
    Utility::ZipStrings(colorInfoBuffer, colordisplayStr, info);

    std::string display;
    Utility::ZipStrings(display, shademapStr, huemapStr);
    display += "\n" + colorInfoBuffer;
    display += "\n\033[38;2;128;128;128m(jk) Hue, (ws) Brightness, (ad) Saturation, (q/ENTER) Done\033[0m";

    std::cerr << display << std::endl;

    // Move cursor up to overwrite
    size_t lines = Utility::CountLines(display);
    std::cerr << "\r\033[" + std::to_string(lines) + "A";
    return display;
}

// -------------------------------------------------------------
// MAIN
// -------------------------------------------------------------
int main(int argc, char* argv[]) {
    auto args = Utility::ParseArgs(argc, argv);

    const std::vector<std::string> acceptedArgs = {"help", "h", "version", "V", "format", "f", "size", "s", "view", "v", "no-wipe", "W"};

    // Check for unknown arguments
    for (const auto& arg : args) {
        bool found = false;
        for (const auto& accepted : acceptedArgs) {
            if (arg.first == accepted) {
                found = true;
                break;
            }
        }
        if (!found) {
            std::cerr << "Error: Unknown argument '" << arg.first << "'\n";
            printUsage();
            return 1;
        }
    }

    // CLI handling
    if (args.count("help") || args.count("h")) { printUsage(); return 0; }
    if (args.count("version") || args.count("V")) { std::cout << "Version: clid " << VERSION << "\n"; return 0; }
    if (args.count("format") || args.count("f")) {
        std::string formatStr = args.count("format") ? args["format"] : args["f"];
        if (!parseFormat(formatStr, state.format)) {
            std::cerr << "Invalid format for --format!\n";
            printUsage();
            return 1;
        }
    }
    if (args.count("size") || args.count("s")) {
        std::string sizeStr = args.count("size") ? args["size"] : args["s"];
        state.xSize = state.ySize = std::stoi(sizeStr);
    }

    if (args.count("no-wipe") || args.count("W")) {
        state.wipeScreen = false;
    }

    // View mode
    if (args.count("view") || args.count("v")) {
        std::string viewStr = args.count("view") ? args["view"] : args["v"];
        Color::RGB rgb;
        switch (state.format) {
            case Format::RGB: {
                auto values = Utility::Split(viewStr, ',');
                if (values.size() != 3 ||
                    !Utility::StringToUint8(values[0], rgb.r) ||
                    !Utility::StringToUint8(values[1], rgb.g) ||
                    !Utility::StringToUint8(values[2], rgb.b)) {
                    std::cerr << "Invalid RGB value for --view!\n";
                    return 1;
                }
                break;
            }
            case Format::HEX:
                if (!Color::HEXtoRGB(rgb, viewStr)) {
                    std::cerr << "Invalid HEX value for --view!\n";
                    return 1;
                }
                break;
            case Format::HSL: {
                auto values = Utility::Split(viewStr, ',');
                if (values.size() != 3) {
                    std::cerr << "Invalid HSL value for --view!\n";
                    return 1;
                }
                Color::HSL hsl;
                hsl.h = std::stof(values[0]) / 100.0f;
                hsl.s = std::stof(values[1]) / 100.0f;
                hsl.l = std::stof(values[2]) / 100.0f;
                Color::HSLtoRGB(rgb, hsl);
                break;
            }
            case Format::CMYK: {
                auto values = Utility::Split(viewStr, ',');
                if (values.size() != 4) {
                    std::cerr << "Invalid CMYK value for --view!\n";
                    return 1;
                }
                Color::CMYK cmyk;
                cmyk.c = std::stof(values[0]) / 100.0f;
                cmyk.m = std::stof(values[1]) / 100.0f;
                cmyk.y = std::stof(values[2]) / 100.0f;
                cmyk.k = std::stof(values[3]) / 100.0f;
                Color::CMYKtoRGB(rgb, cmyk);
                break;
            }
        }
        viewColor(rgb);
        return 0;
    }

    // Interactive mode
    Input::Manager inputManager;
    for (char c : {'k','j','q','w','a','s','d','\n'})
        inputManager.addEvent(c, handleInput);

    Color::RGB finalColor;
    std::string display;

    while (state.running) {
        display = drawUI();
        finalColor = Render::GetShadeColor(state.xSize, state.ySize, state.hue.h, state.selectedX, state.selectedY);
        inputManager.update();
    }

    if (state.wipeScreen) {
        size_t lines = Utility::CountLines(display);

        // Overwrite all lines
        for (size_t l = 0; l < lines; l++) {
            std::cerr << "\r\033[2K";
            if (l < lines - 1) std::cerr << "\n";
        }

        // Move cursor back up to the top of cleared area
        std::cerr << "\033[" << lines << "A" << "\n";
    } else {
        size_t lines = Utility::CountLines(display);
        Utility::CursorDown(std::cerr, lines);
    }

    // Final output based on format
    switch (state.format) {
        case Format::RGB:
            std::cout << (int)finalColor.r << "," << (int)finalColor.g << "," << (int)finalColor.b << "\n";
            break;
        case Format::HEX: {
            Color::HEX hexColor;
            Color::RGBtoHEX(hexColor, finalColor);
            std::cout << hexColor << "\n";
            break;
        }
        case Format::HSL: {
            Color::HSL hsl;
            Color::RGBtoHSL(hsl, finalColor);
            std::cout << hsl.h * 100.0f << "," << hsl.s * 100.0f << "," << hsl.l * 100.0f << "\n";
            break;
        }
        case Format::CMYK: {
            Color::CMYK cmyk;
            Color::RGBtoCMYK(cmyk, finalColor);
            std::cout << cmyk.c * 100.0f << "," << cmyk.m * 100.0f << "," << cmyk.y * 100.0f << "," << cmyk.k * 100.0f << "\n";
            break;
        }
    }
}
