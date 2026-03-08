# Integrity

An Ada program that runs on every ecosystem startup before the launcher presents its menu. Part of the research_ecosystem offline cloud.

## What it does

Checks that all critical folders and the database file exist and are in place. If anything is missing it reports exactly what failed and returns a failure code to the launcher, which then asks the user whether to proceed.

## What it checks

- DB/data/database.db - the main SQLite database
- DB/pdf_store - the file storage folder
- Contacts - the contacts folder
- Photos - the photos folder

## Why Ada

Ada was chosen for this layer specifically because its type system and contract-based programming make certain classes of errors impossible to write by accident. For a component whose entire job is to verify that data is intact, using a language that enforces correctness at compile time is the right tool.

## Requirements

You need an Ada compiler to build this. Two options:

**Option 1 - Alire (recommended, handles everything automatically):**
```bash
# macOS
brew install alire

# Linux
# download from https://alire.ada.dev
```

**Option 2 - GNAT directly (simpler, no package manager):**
```bash
# macOS
brew install gnat

# openSUSE
sudo zypper install gcc-ada

# Debian/Ubuntu
sudo apt install gnat
```

## How to build

**With Alire:**
```bash
cd integrity
alr build
```

**With GNAT directly:**
```bash
gnatmake integrity_check.adb
```

## How to run

**With Alire:**
```bash
alr run integrity
```

**With GNAT directly:**
```bash
./integrity_check
```

## Exit codes

- 0 - all checks passed
- 1 - one or more checks failed

The launcher reads these exit codes automatically. You do not need to run this manually unless debugging.

## Part of research_ecosystem

This is the integrity layer of research_ecosystem, a fully offline personal cloud designed to run from a USB drive with no internet dependency.
