#!/bin/bash

echo "--- research_ecosystem setup ---"
BASE="$(cd "$(dirname "$0")" && pwd)"

# build C DB
echo "Building DB..."
cd "$BASE/DB"
gcc main.c -o CastorimorphaDB -lsqlite3
if [ $? -eq 0 ]; then
    echo "DB built."
else
    echo "DB build failed. Make sure gcc and sqlite3 are installed."
fi

# build Ada integrity checker
echo "Building integrity checker..."
cd "$BASE/integrity"

# try Alire first, fall back to gnatmake
if command -v alr &> /dev/null; then
    echo "Alire found, using alr build..."
    alr build
else
    echo "Alire not found, trying gnatmake..."
    if command -v gnatmake &> /dev/null; then
        gnatmake integrity.adb
    else
        echo "No Ada compiler found. Install Alire from https://alire.ada.dev or GNAT via your package manager."
    fi
fi

echo "Integrity checker built."

# install Go dependencies for radio app
echo "Setting up radio app..."
cd "$BASE/radio_go_app"
go mod tidy
echo "Radio app ready."

echo ""
echo "Setup complete. Run with: perl Launcher/launcher.pl"
