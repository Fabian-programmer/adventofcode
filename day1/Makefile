
# Compiler
CXX = g++-10

# Compiler flags
CXXFLAGS = -std=c++2a -O3 -Wall

# Source file
SOURCE = stringread.cpp

# Output executable
OUTPUT = stringread.out

# Build rule
all: $(OUTPUT)

$(OUTPUT): $(SOURCE)
	$(CXX) $(CXXFLAGS) $< -o $@

# Clean rule
clean:
	rm -f $(OUTPUT)
