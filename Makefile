CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -O2
TARGET = clid
SRC = src/main.cpp src/Render.cpp src/Input.cpp src/Utility.cpp src/Color.cpp

all: build/$(TARGET)

build/:
	mkdir build

build/$(TARGET): $(SRC) build/
	$(CXX) $(CXXFLAGS) $(SRC) -o build/$(TARGET)

clean:
	rm -rf build

install: build/$(TARGET)
	sudo install -Dm755 build/clid /usr/local/bin/clid

uninstall:
	sudo rm /usr/local/bin/clid