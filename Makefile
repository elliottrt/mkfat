CPP=g++
CPPFLAGS=-Wall -Wextra -Wpedantic -std=c++17
OUT=mkfat

CPPSRC=$(wildcard src/*.cpp)
CPPHDR=$(wildcard src/*.h)

all: $(OUT)

$(OUT): $(CPPSRC) $(CPPHDR) ./Makefile
	$(CPP) -o $(OUT) $(CPPSRC) $(CPPFLAGS)
