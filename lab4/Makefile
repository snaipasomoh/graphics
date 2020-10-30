CC = g++
FLAGS = -lGLEW -lglfw -lGL -lSOIL

all: main.cpp shader.hpp
	$(CC) main.cpp $(FLAGS) -o foo

.PHONY: clear

clear:
	$(RM) foo