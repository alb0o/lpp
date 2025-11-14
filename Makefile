# Simple Makefile for LPP compiler
# Requires: g++ or clang++ with C++17 support

CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -I./include
SRCDIR := src
OBJDIR := build/obj
BINDIR := build
TARGET := $(BINDIR)/lppc

# Source files
SOURCES := $(wildcard $(SRCDIR)/*.cpp)
OBJECTS := $(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

# Default target
all: directories $(TARGET)

# Create directories
directories:
	@mkdir -p $(OBJDIR)
	@mkdir -p $(BINDIR)

# Link
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^
	@echo "Build successful! Executable: $(TARGET)"

# Compile
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean
clean:
	rm -rf $(OBJDIR) $(TARGET)
	@echo "Clean complete"

# Rebuild
rebuild: clean all

# Run tests
test: $(TARGET)
	@echo "Running hello.lpp example..."
	./$(TARGET) examples/hello.lpp -o build/hello
	./build/hello

# Help
help:
	@echo "LPP Compiler Makefile"
	@echo "Usage:"
	@echo "  make          - Build the compiler"
	@echo "  make clean    - Remove build artifacts"
	@echo "  make rebuild  - Clean and rebuild"
	@echo "  make test     - Build and run tests"
	@echo "  make help     - Show this help"

.PHONY: all directories clean rebuild test help
