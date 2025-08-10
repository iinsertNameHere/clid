#pragma once

#include <vector>

namespace Input {
    typedef void (*Handler)(char&);

    struct Event {
        const char key;
        Handler eventHandler;
    };

    class Manager {
    private:
        std::vector<Event> events;
    public:
        void addEvent(const char key, Handler eventHandler);

        void update();
    };
}