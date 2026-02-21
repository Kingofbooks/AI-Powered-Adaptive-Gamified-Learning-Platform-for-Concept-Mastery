# Educational Physics Sandbox - Graphical Version Makefile

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
SDL_FLAGS = `sdl2-config --cflags --libs` -lSDL2_ttf
TARGET = EduSandbox
SOURCES = main.cpp Entities.cpp PhysicsEngine.cpp PromptParser.cpp GameEngine.cpp
OBJECTS = $(SOURCES:.cpp=.o)
HEADERS = EduSandbox.h PhysicsEngine.h PromptParser.h

# Default target
all: check-sdl $(TARGET)

# Check if SDL2 is installed
check-sdl:
	@which sdl2-config > /dev/null || (echo "ERROR: SDL2 not found. Please install SDL2 and SDL2_ttf:" && \
	echo "  Ubuntu/Debian: sudo apt-get install libsdl2-dev libsdl2-ttf-dev" && \
	echo "  macOS: brew install sdl2 sdl2_ttf" && \
	echo "  Fedora: sudo dnf install SDL2-devel SDL2_ttf-devel" && exit 1)

# Link executable
$(TARGET): $(OBJECTS)
	@echo "Linking $(TARGET)..."
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJECTS) $(SDL_FLAGS)
	@echo "Build complete! Run with ./$(TARGET)"

# Compile source files
%.o: %.cpp $(HEADERS)
	@echo "Compiling $<..."
	$(CXX) $(CXXFLAGS) `sdl2-config --cflags` -c $< -o $@

# Clean build files
clean:
	@echo "Cleaning..."
	rm -f $(OBJECTS) $(TARGET)
	@echo "Clean complete!"

# Rebuild
rebuild: clean all

# Run the program
run: $(TARGET)
	./$(TARGET)

# Debug build
debug: CXXFLAGS = -std=c++17 -Wall -Wextra -g -O0
debug: rebuild

# Install SDL2 (helper targets)
install-sdl-ubuntu:
	sudo apt-get update
	sudo apt-get install -y libsdl2-dev libsdl2-ttf-dev

install-sdl-macos:
	brew install sdl2 sdl2_ttf

install-sdl-fedora:
	sudo dnf install -y SDL2-devel SDL2_ttf-devel

# Help
help:
	@echo "Educational Physics Sandbox - Build System"
	@echo ""
	@echo "Targets:"
	@echo "  make              - Build the program"
	@echo "  make run          - Build and run"
	@echo "  make clean        - Remove build files"
	@echo "  make rebuild      - Clean and rebuild"
	@echo "  make debug        - Build with debug symbols"
	@echo ""
	@echo "SDL2 Installation:"
	@echo "  make install-sdl-ubuntu   - Install SDL2 on Ubuntu/Debian"
	@echo "  make install-sdl-macos    - Install SDL2 on macOS"
	@echo "  make install-sdl-fedora   - Install SDL2 on Fedora"
	@echo ""
	@echo "Requirements:"
	@echo "  - SDL2 library"
	@echo "  - SDL2_ttf library"
	@echo "  - SDL2_image library (optional: PNG/JPEG support)"
	@echo "  - C++17 compatible compiler"

.PHONY: all clean rebuild run debug check-sdl help install-sdl-ubuntu install-sdl-macos install-sdl-fedora
