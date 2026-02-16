#!/bin/bash
# Educational Physics Sandbox - Universal Build Script
# Works on Ubuntu, Debian, Fedora, and macOS

set -e  # Exit on error

echo "╔════════════════════════════════════════════╗"
echo "║  Educational Physics Sandbox - Builder    ║"
echo "║  Universal Installation Script             ║"
echo "╚════════════════════════════════════════════╝"
echo ""

# Detect OS
detect_os() {
    if [[ "$OSTYPE" == "linux-gnu"* ]]; then
        if [ -f /etc/debian_version ]; then
            OS="debian"
            echo "🐧 Detected: Ubuntu/Debian Linux"
        elif [ -f /etc/fedora-release ]; then
            OS="fedora"
            echo "🐧 Detected: Fedora/RHEL Linux"
        else
            OS="linux"
            echo "🐧 Detected: Generic Linux"
        fi
    elif [[ "$OSTYPE" == "darwin"* ]]; then
        OS="macos"
        echo "🍎 Detected: macOS"
    else
        OS="unknown"
        echo "❓ Unknown OS: $OSTYPE"
    fi
}

# Check dependencies
check_sdl2() {
    echo ""
    echo "📦 Checking for SDL2..."
    
    if command -v sdl2-config &> /dev/null; then
        SDL2_VERSION=$(sdl2-config --version)
        echo "   ✓ SDL2 found: version $SDL2_VERSION"
        return 0
    else
        echo "   ✗ SDL2 not found"
        return 1
    fi
}

check_ttf() {
    if pkg-config --exists SDL2_ttf 2>/dev/null; then
        echo "   ✓ SDL2_ttf found"
        return 0
    else
        echo "   ✗ SDL2_ttf not found"
        return 1
    fi
}

check_compiler() {
    echo ""
    echo "🔧 Checking for C++ compiler..."
    
    if command -v g++ &> /dev/null; then
        GCC_VERSION=$(g++ --version | head -n1)
        echo "   ✓ g++ found: $GCC_VERSION"
        return 0
    elif command -v clang++ &> /dev/null; then
        CLANG_VERSION=$(clang++ --version | head -n1)
        echo "   ✓ clang++ found: $CLANG_VERSION"
        return 0
    else
        echo "   ✗ No C++ compiler found"
        return 1
    fi
}

# Install dependencies
install_debian() {
    echo ""
    echo "📥 Installing dependencies for Ubuntu/Debian..."
    echo ""
    
    sudo apt-get update
    sudo apt-get install -y \
        libsdl2-dev \
        libsdl2-ttf-dev \
        g++ \
        make \
        fonts-dejavu-core
    
    echo ""
    echo "✓ Dependencies installed!"
}

install_fedora() {
    echo ""
    echo "📥 Installing dependencies for Fedora/RHEL..."
    echo ""
    
    sudo dnf install -y \
        SDL2-devel \
        SDL2_ttf-devel \
        gcc-c++ \
        make \
        dejavu-sans-fonts
    
    echo ""
    echo "✓ Dependencies installed!"
}

install_macos() {
    echo ""
    echo "📥 Installing dependencies for macOS..."
    echo ""
    
    # Check for Homebrew
    if ! command -v brew &> /dev/null; then
        echo "❌ Homebrew not found!"
        echo ""
        echo "Please install Homebrew first:"
        echo "  /bin/bash -c \"\$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)\""
        echo ""
        exit 1
    fi
    
    brew install sdl2 sdl2_ttf
    
    echo ""
    echo "✓ Dependencies installed!"
}

# Build the project
build_project() {
    echo ""
    echo "🔨 Building Educational Physics Sandbox..."
    echo ""
    
    # Clean previous build
    if [ -f "Makefile" ]; then
        make clean 2>/dev/null || true
    fi
    
    # Build
    make
    
    if [ $? -eq 0 ]; then
        echo ""
        echo "╔════════════════════════════════════════════╗"
        echo "║        BUILD SUCCESSFUL! 🎉                ║"
        echo "╚════════════════════════════════════════════╝"
        echo ""
        echo "Run the program with:"
        echo "  ./EduSandbox"
        echo ""
        return 0
    else
        echo ""
        echo "❌ Build failed!"
        echo ""
        echo "Please check the error messages above."
        echo "If you need help, check README.md or INSTALL.md"
        echo ""
        return 1
    fi
}

# Main installation flow
main() {
    detect_os
    
    # Check if already installed
    SDL2_OK=false
    TTF_OK=false
    COMPILER_OK=false
    
    check_sdl2 && SDL2_OK=true || SDL2_OK=false
    check_ttf && TTF_OK=true || TTF_OK=false
    check_compiler && COMPILER_OK=true || COMPILER_OK=false
    
    # Determine what to install
    if $SDL2_OK && $TTF_OK && $COMPILER_OK; then
        echo ""
        echo "✓ All dependencies already installed!"
        echo ""
        read -p "Build the project now? [Y/n] " -n 1 -r
        echo
        if [[ $REPLY =~ ^[Yy]$ ]] || [[ -z $REPLY ]]; then
            build_project
        fi
    else
        echo ""
        echo "⚠️  Missing dependencies detected"
        echo ""
        
        if [[ $OS == "unknown" ]]; then
            echo "❌ Cannot auto-install on this OS"
            echo ""
            echo "Please manually install:"
            echo "  - SDL2 development libraries"
            echo "  - SDL2_ttf development libraries"
            echo "  - C++ compiler (g++ or clang++)"
            echo "  - Make build tool"
            echo ""
            exit 1
        fi
        
        read -p "Install missing dependencies? [Y/n] " -n 1 -r
        echo
        
        if [[ $REPLY =~ ^[Yy]$ ]] || [[ -z $REPLY ]]; then
            case $OS in
                debian)
                    install_debian
                    ;;
                fedora)
                    install_fedora
                    ;;
                macos)
                    install_macos
                    ;;
            esac
            
            echo ""
            read -p "Build the project now? [Y/n] " -n 1 -r
            echo
            if [[ $REPLY =~ ^[Yy]$ ]] || [[ -z $REPLY ]]; then
                build_project
            fi
        else
            echo ""
            echo "Installation cancelled."
            echo "Run this script again when ready to install."
            echo ""
        fi
    fi
}

# Run main
main
