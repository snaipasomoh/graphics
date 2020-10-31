CC = g++
FLAGS = -std=c++17 -lGLEW -lglfw -lGL -lSOIL -lassimp

all: main.cpp shader.hpp
	$(CC) main.cpp $(FLAGS) -o foo

.PHONY: clear

clear:
	$(RM) foo