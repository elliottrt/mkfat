CXXFLAGS:=-Wall -Wextra -Wpedantic -std=c++17
TARGET=mkfat

CXXSRC=$(wildcard src/*.cpp)
CXXHDR=$(wildcard src/*.h)

.PHONY: all

all: $(TARGET)

$(TARGET): $(CXXSRC) $(CXXHDR) ./Makefile
	$(CXX) -o $(TARGET) $(CXXSRC) $(CXXFLAGS)
