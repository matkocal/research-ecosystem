#!/bin/bash

echo "--- research_ecosystem setup ---"
BASE="$(cd "$(dirname "$0")" && pwd)"

# build C DB
echo "Building DB..."
cd "$BASE/DB"
gcc main.c -o CastorimorphaDB -lsqlite3
echo "DB built."

# build Ada integrity checker
echo "Building integrity checker..."
cd "$BASE/integrity"
gnatmake integrity_check.adb
echo "Integrity checker built."

# install Go dependencies for radio app
echo "Setting up radio app..."
cd "$BASE/radio_go_app"
go mod tidy
echo "Radio app ready."

echo ""
echo "Setup complete. Run with: perl launcher/launcher.pl"
