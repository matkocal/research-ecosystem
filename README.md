# research_ecosystem

A simple offgrid alternative to cloud services.

Built entirely from scratch across C, Perl, Go, and Ada. Runs from a USB drive with no internet dependency, no accounts, no third party services.

## Components

- DB - C-based file and knowledge database with SQLite, group hierarchy, and tagging
- Photos - Perl-based photo manager for storing, organizing and viewing image files
- Contacts - Perl-based contact manager storing data as plain text files
- radio_go_app - Go-based communication layer over HAM packet radio via AX.25


## Glue Layer

A Perl launcher script ties all components together into a single entry point. On startup it presents a menu and launches the chosen app from the correct path, regardless of which machine or USB port the ecosystem is mounted on.

## Integrity Layer

An Ada program runs before the launcher on every startup. It checks that all critical folders and the database file exist and are in place before the ecosystem is allowed to start. Ada was chosen for this layer specifically because its type system and contract-based programming make certain classes of errors impossible to write by accident.

## Philosophy

Everything runs offline. Data lives on a physical USB drive you own. No subscription, no cloud vendor, no network required. Communicates over HAM radio frequencies when internet infrastructure is unavailable. Designed to function with solar power as the only energy source.

## Requirements

- Linux or macOS
- Perl 5
- GCC and SQLite3 for the DB component
- Go for the radio component
- GNAT for the Ada integrity layer

## How to run each component

See the README.md inside each subfolder.
