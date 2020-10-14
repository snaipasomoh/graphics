CC = g++
FLAGS = -lGLEW -lglfw -lGL

all: main.cpp shader.hpp camera.hpp
	$(CC) main.cpp $(FLAGS) -o foo

.PHONY: clear

clear:
	$(RM) foo