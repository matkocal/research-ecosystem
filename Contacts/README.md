# Contacts App

A minimal command-line contacts manager written in Perl. Part of the research_ecosystem offline cloud.

## What it does

Stores contacts as plain text files on disk. No database, no dependencies beyond Perl itself. Each contact is a .txt file in the contacts/ directory, making the data portable, readable, and easy to back up.

## Requirements

- Perl 5 (pre-installed on any Linux or macOS system)

## How to run

```bash
perl contacts2.pl
```

## Commands

1. Add contact - creates a new contact with name, number, email, and additional info
2. List all contacts - prints every contact stored
3. Delete a contact - removes a contact by name
4. Search a contact - finds contacts by partial or full name match
5. Exit - closes the program

## Data storage

Each contact is stored as a plain .txt file in the contacts/ directory using key=value format:

```
name=John Doe
number=+421900000000
email=john@example.com
other=met at conference
```

The filename is the contact name with spaces replaced by underscores, for example John_Doe.txt.

## Portability

The contacts/ directory is self-contained. Copy it anywhere, plug in the USB, run the script. No setup required.

## Part of research_ecosystem

This app is one component of research_ecosystem, a fully offline personal cloud designed to run from a USB drive with no internet dependency. Other components include a C-based file database, a photo organizer, a Go-based packet radio communication layer, and a website backup system.
