#!/bin/bash

# Teacher AI Agent Startup Script for macOS/Linux

echo ""
echo "========================================"
echo "Teacher AI Agent - AI-Powered Gaming Platform"
echo "========================================"
echo ""

# Check if Python is installed
if ! command -v python3 &> /dev/null; then
    echo "ERROR: Python 3 is not installed"
    echo "Please install Python 3.8+ from https://www.python.org"
    exit 1
fi

echo "[1/4] Checking Python version..."
python3 --version

# Check if virtual environment exists
if [ ! -d "venv" ]; then
    echo "[2/4] Creating virtual environment..."
    python3 -m venv venv
else
    echo "[2/4] Virtual environment already exists"
fi

# Activate virtual environment
echo "[3/4] Activating virtual environment..."
source venv/bin/activate

# Install dependencies
echo "[4/4] Installing dependencies..."
pip install -r requirements.txt

echo ""
echo "========================================"
echo "Starting Teacher AI Agent Server..."
echo "========================================"
echo ""
echo "API will be available at: http://localhost:8000"
echo "Documentation at: http://localhost:8000/docs"
echo ""

# Run the server
python main.py
