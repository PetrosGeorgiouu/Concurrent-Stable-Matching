CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -Wpedantic -Iinclude

TARGET := gale_shapley_tests
SOURCES := src/GaleShapley.cpp tests/testGS.cpp
HEADERS := include/GaleShapley.hpp

.PHONY: test clean

test: $(TARGET)
	./$(TARGET)

$(TARGET): $(SOURCES) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(TARGET)

clean:
	$(RM) $(TARGET)
