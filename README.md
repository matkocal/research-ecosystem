# research_ecosystem

A simple offgrid alternative to cloud services.

Built entirely from scratch across C, Perl, Go, and Ada. Runs from a USB drive with no internet dependency, no accounts, no third party services.

## Components

- DB - C-based file and knowledge database with SQLite, group hierarchy, and tagging
- photos - Perl-based photo organizer using ExifTool for metadata management
- contacts - Perl-based contact manager storing data as plain text files
- radio (coming) - Go-based communication layer over HAM packet radio via AX.25
- sync (coming) - C++ sync engine for USB-to-USB data portability

## Philosophy

Everything runs offline. Data lives on a physical USB drive you own. No subscription, no cloud vendor, no network required. Communicates over HAM radio frequencies when internet infrastructure is unavailable.

## Requirements

- Linux or macOS
- Perl 5
- GCC and SQLite3 for the DB component
- Go for the radio component (coming)

## How to run each component

See the README.md inside each subfolder.
