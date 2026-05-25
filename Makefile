CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -g

TARGET = language
SRC = main.cpp $(wildcard ./src/*.cpp)

all:
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

build: all

run: all
	./$(TARGET)

clean:
	rm -f $(TARGET)
