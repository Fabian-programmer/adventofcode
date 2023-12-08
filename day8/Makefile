# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -std=c++17 -O3 -Wall

# Source file
SOURCE = maze.cpp

# Output executable
OUTPUT = maze.out

# Build rule
all: $(OUTPUT)

$(OUTPUT): $(SOURCE)
	$(CXX) $(CXXFLAGS) $< -o $@

# Clean rule
clean:
	rm -f $(OUTPUT)
