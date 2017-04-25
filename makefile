# A simple makefile for linux version

CXX=g++
CFLAG=-std=c++14 -g -Wall -Wextra -D_DEBUG
LIB=-lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio

CPP=$(wildcard src/*.cpp)
OBJ=$(patsubst src/%.cpp,obj/%.o,$(CPP))

obj/%.o: src/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CFLAG) -c $< -o $@

bin/sfml-game: $(OBJ)
	mkdir -p bin/data/
	cp -r -u src/data/ bin/
	$(CXX) -pthread $^ -o $@ $(LIB)

.PHONY : clean
clean :
	rm -f -r bin/
	rm -f -r obj/
