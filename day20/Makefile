# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -std=c++17 -g -O3 -Wall

# Source file
SOURCE = broadcast.cpp

# Output executable
OUTPUT = broadcast.out

# Build rule
all: $(OUTPUT)

$(OUTPUT): $(SOURCE)
	$(CXX) $(CXXFLAGS) $< -o $@

# Clean rule
clean:
	rm -f $(OUTPUT)
