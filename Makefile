CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -O2
TARGET = clid
SRC = src/main.cpp src/PPM.cpp src/Input.cpp src/Utility.cpp

all: $(TARGET)

$(TARGET): $(SRC) build/
	$(CXX) $(CXXFLAGS) $(SRC) -o build/$(TARGET)

build/:
	mkdir build

clean:
	rm -rf build