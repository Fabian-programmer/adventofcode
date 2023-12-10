# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -std=c++17 -g -O0 -Wall

# Source file
SOURCE = piping.cpp

# Output executable
OUTPUT = piping.out

# Build rule
all: $(OUTPUT)

$(OUTPUT): $(SOURCE)
	$(CXX) $(CXXFLAGS) $< -o $@

# Clean rule
clean:
	rm -f $(OUTPUT)
