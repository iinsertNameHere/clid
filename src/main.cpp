#include "PPM.h"
#include "Input.h"
#include "Utility.h"
#include <string>
#include <iostream>
#include <sstream>
#include <vector>

#define VERSION "v0.1.0"

int X_SIZE = 25;
int Y_SIZE = 25;

float hue = 0.0f;
int selectedX = 0;
int selectedY = 0;
bool runing = true;
void inputHandler(char& key) {
    switch (key) {
        case 'k': {
            hue += 0.01f;
            if (hue > 1.0f) hue = 1.0f;
            break;
        };
        case 'j': {
            hue -= 0.01f;
            if (hue < 0.0f) hue = 0.0f;
            break;
        };
        case 'w': {
            selectedY -= 1;
            if (selectedY < 0) selectedY = 0;
            break;
        };
        case 'a': {
            selectedX -= 1;
            if (selectedX < 0) selectedX = 0;
            break;
        };
        case 's': {
            selectedY += 1;
            if (selectedY > Y_SIZE - 1) selectedY = Y_SIZE - 1;
            break;
        };
        case 'd': {
            selectedX += 1;
            if (selectedX > X_SIZE - 1) selectedX = X_SIZE - 1;
            break;
        };
        case 'q': {
            runing = false;
            break;
        };
        default: break;
    }
}

void ViewColor(PPM::Pixel& color) {
    PPM::Image colorView;
    colorView.width = 4;
    colorView.height = 4;

    PPM::Fill(colorView, color);

    std::string colorviewBuffer;
    PPM::ToString(colorviewBuffer, colorView);

    std::string colorInfo;
    colorInfo += "RGB: " + std::to_string(color.r) + ", " + std::to_string(color.g) + ", " + std::to_string(color.b) + "\n";
    colorInfo += "HEX: " + Utility::RGBtoHEX(color.r, color.g, color.b);

    std::string buffer;
    Utility::ZipStrings(buffer, colorviewBuffer, colorInfo);

    std::cout << buffer << "\n";
}

void usage() {
    std::cout << "Usage: clid [ARGUMENTS]\n\n";
    std::cout << "ARGUMENTS:\n";
    std::cout << "    --help        Show this help list.\n";
    std::cout << "    --version     Show version number\n";
    std::cout << "    --rgb={rgb}   View a color in rgb format.\n";
    std::cout << "    --hex={hex}   View a color in hex format.\n";
    std::cout << "    --size={num}  Number of pixels for width and hight.\n";
    std::cout << "    --output-hex  Set output format to be hex instead of rgb.\n";
    std::cout << "\n";
    std::cout << "TUI CONTROLLS:\n";
    std::cout << "    j  Move hue selector up.\n";
    std::cout << "    k  Move hue selector down.\n";
    std::cout << "    w  Move shade selector up.\n";
    std::cout << "    a  Move shade selector left.\n";
    std::cout << "    s  Move shade selector down.\n";
    std::cout << "    d  Move shade selector right.\n";
    std::cout << "    q  Exit\n";
}

int main(int argc, char* argv[]) {
    auto args = Utility::ParseArgs(argc, argv);

    bool viewColor = false;
    PPM::Pixel viewColor_Color = {0, 0, 0};

    bool hexOutput = false;

    if (args.count("help")) {
        usage();
        return 0;
    }
    if (args.count("version")) {
        std::cout << "Version: clid " << VERSION << "\n";
        return 0;
    }
    if (args.count("output-hex")) hexOutput = true;
    if (args.count("rgb")) {
        if (viewColor) {
            std::cerr << "Argument --rgb can't be used together with --hex!\n";
            usage();
            return 1;
        }

        if (args["rgb"] == "") {
            std::cerr << "No value provied for --rgb!\n";
            usage();
            return 1;
        }
        std::vector<std::string> values = Utility::Split(args["rgb"], ',');
        if (values.size() != 3) {
            std::cerr << "Value provied for --rgb has the wrong format! \n";
            std::cerr << "Correct Format: {num},{num},{num}\n";
            usage();
            return 1;
        }
        uint8_t r, g, b;
        if (
            !Utility::StringToUint8(values[0], r) ||
            !Utility::StringToUint8(values[1], g) ||
            !Utility::StringToUint8(values[2], b)
        ) {
            std::cerr << "Value provied for --rgb has the wrong format! \n";
            std::cerr << "Correct Format: {num},{num},{num}\n";
            usage();
            return 1;
        }

        viewColor_Color = {r, g, b};
        viewColor = true;
    }
    if (args.count("hex")) {
        if (viewColor) {
            std::cerr << "Argument --hex can't be used together with --rgb!\n";
            usage();
            return 1;
        }

        if (args["hex"] == "") {
            std::cerr << "No value provied for --hex!\n";
            usage();
            return 1;
        }

        uint8_t r, g, b;

        if (!Utility::HEXtoRGB(args["hex"], r, g, b)) {
            std::cerr << "Value provied for --hex has the wrong format! \n";
            std::cerr << "Correct Format: #{hexnum}\n";
            usage();
            return 1;
        }

        viewColor_Color = {r, g, b};
        viewColor = true;
    }
    if (args.count("size")) {
        if (viewColor) {
            std::cerr << "Argument --size can't be used together with --rgb or --hex!\n";
            usage();
            return 1;
        }

        if (args["size"] == "") {
            std::cerr << "No value provied for --size!\n";
            usage();
            return 1;
        }

        int size = std::stoi(args["size"]);
        X_SIZE = size;
        Y_SIZE = size;
    }

    if (viewColor) {
        ViewColor(viewColor_Color);
        return 0;
    }

    Input::Manager inputManager;
    inputManager.addEvent('k', inputHandler);
    inputManager.addEvent('j', inputHandler);
    inputManager.addEvent('q', inputHandler);
    inputManager.addEvent('w', inputHandler);
    inputManager.addEvent('a', inputHandler);
    inputManager.addEvent('s', inputHandler);
    inputManager.addEvent('d', inputHandler);

    std::string displayBuffer;

    std::string shademapBuffer;
    PPM::Image shademap;
    shademap.height = Y_SIZE;
    shademap.width = X_SIZE;

    std::string huemapBuffer;
    PPM::Image huemap;
    huemap.height = Y_SIZE;
    huemap.width = 4;

    PPM::Pixel selectedColor;
    std::string colordisplayBuffer;
    PPM::Image colordisplay;
    colordisplay.width = 4;
    colordisplay.height = 4;

    while (runing) {
        displayBuffer = "";

        shademapBuffer = "";
        shademap.pixelMatrix.clear();

        huemapBuffer = "";
        huemap.pixelMatrix.clear();

        colordisplayBuffer = "";
        colordisplay.pixelMatrix.clear();

        PPM::GenerateShadeMap(shademap, hue);
        PPM::GenerateHueMap(huemap);

        selectedColor = shademap.pixelMatrix[selectedY][selectedX];
        PPM::Fill(colordisplay, selectedColor);

        shademap.pixelMatrix[selectedY][selectedX] = PPM::HSLtoRGB(hue + 0.1f, 0.8f, 0.8f);

        size_t l = 0;
        bool highlighted = false;
        while (l < huemap.height - 1) {
            float hue1 = PPM::RGBtoHSL(huemap.pixelMatrix[l][0]).h;
            float hue2 = PPM::RGBtoHSL(huemap.pixelMatrix[l + 1][0]).h;

            if ((hue >= hue1 && hue <= hue2) || (hue >= hue2 && hue <= hue1)) {
                std::vector<PPM::Pixel> selectedRow;
                for (size_t i = 0; i < huemap.width; i++) {
                    selectedRow.push_back(PPM::HSLtoRGB(hue1, 0.4f, 0.4f));
                }
                huemap.pixelMatrix[l] = selectedRow;
                highlighted = true;
                break;
            }
            ++l;
        }

        if (!highlighted) {
            // Check the last line alone:
            float lastHue = PPM::RGBtoHSL(huemap.pixelMatrix[huemap.height - 1][0]).h;
            // Optionally check if hue is close enough to lastHue or just highlight anyway
            if (true) { // or some condition on hue vs lastHue
                std::vector<PPM::Pixel> selectedRow;
                for (size_t i = 0; i < huemap.width; i++) {
                    selectedRow.push_back(PPM::HSLtoRGB(lastHue, 0.3f, 0.3f));
                }
                huemap.pixelMatrix[huemap.height - 1] = selectedRow;
            }
        }

        PPM::ToString(shademapBuffer, shademap);
        PPM::ToString(huemapBuffer, huemap);
        PPM::ToString(colordisplayBuffer, colordisplay);

        std::string colorInfo;
        colorInfo += "RGB: " + std::to_string(selectedColor.r) + " " + std::to_string(selectedColor.g) + " " + std::to_string(selectedColor.b) + "\n";
        colorInfo += "HEX: " + Utility::RGBtoHEX(selectedColor.r, selectedColor.g, selectedColor.b);

        std::string selectedColorInfoBuffer;
        Utility::ZipStrings(selectedColorInfoBuffer, colordisplayBuffer, colorInfo);

        Utility::ZipStrings(displayBuffer, shademapBuffer, huemapBuffer);
        displayBuffer += "\n" + selectedColorInfoBuffer;
        
        std::cout << "\n" << displayBuffer << std::endl;

        size_t lines = Utility::CountLines(displayBuffer) + 1;
        std::cout << "\r\033[" + std::to_string(lines) + "A";

        inputManager.update();
    }
    
    size_t lines = Utility::CountLines(displayBuffer) + 1;
    for (size_t l = 0; l < lines; l++) {
        for (int i = 0; i < X_SIZE + 5; i++) std::cout << " ";
        std::cout << "\n";
    }
    std::cout << "\r\033[" + std::to_string(lines) + "A" << "\n";

    if (!hexOutput) std::cout << std::to_string(selectedColor.r) << " " << std::to_string(selectedColor.g) << " " << std::to_string(selectedColor.b) << "\n";
    else std::cout << Utility::RGBtoHEX(selectedColor.r, selectedColor.g, selectedColor.b) << "\n";
}