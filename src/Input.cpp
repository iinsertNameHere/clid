#include "Input.h"
#include <iostream>
#include <termios.h>
#include <unistd.h>

int getch() {
    struct termios oldt, newt;
    int ch;

    // Get current terminal settings
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;

    // Disable canonical mode and echo
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    // Read one character
    ch = getchar();

    // Restore terminal settings
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

    return ch;
}


void Input::Manager::addEvent(const char key, Input::Handler eventHandler) {
    events.push_back({key, eventHandler});
}

void Input::Manager::update() {
    char key = getch();

    size_t eventCount = events.size();
    for (size_t i = 0; i < eventCount; i++) {
        Input::Event event = events[i];
        if (event.key == key) (*event.eventHandler)(key);
    }
}