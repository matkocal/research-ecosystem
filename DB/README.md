# Research Ecosystem

A lightweight command-line file management system for organizing research papers, documents, and files using hierarchical groups and tags.

## Features

- **File Management**: Add, delete, rename, and open files from the command line
- **Group Organization**: Create nested groups to organize files hierarchically
- **Search & Filter**: Search files by name and mark important files with stars
- **Multiple Groups**: Files can belong to multiple groups simultaneously
- **SQLite Database**: Fast and reliable local storage
- **Cross-Platform**: Works on macOS, Linux, and Windows

## Installation

### Prerequisites

- C compiler (gcc or clang)
- SQLite3 development libraries
- PDF viewer (optional, for opening files)

### macOS

```bash
# Install SQLite if needed
brew install sqlite3

# Compile
gcc DB.c -o research -lsqlite3

# Run
./research
```

### Linux

```bash
# Install SQLite development libraries
sudo apt-get install libsqlite3-dev  # Ubuntu/Debian
sudo dnf install sqlite-devel        # Fedora
sudo pacman -S sqlite                # Arch

# Compile
gcc DB.c -o research -lsqlite3

# Run
./research
```

### Windows

```bash
# Install SQLite and compile with MinGW or MSVC
gcc DB.c -o research.exe -lsqlite3
research.exe
```

## Usage

### Basic Commands

```bash
# Start the program
./CastorimorphaDB

# Add a file to a group
> add /path/to/paper.pdf research

# Create a new group
> mkdir machine-learning

# List all files
> list

# List all groups
> listgroups

# List files in a specific group
> listgroup research

# Show group hierarchy with files
> groups
```

### File Operations

```bash
# Open a file with default application
> open paper.pdf

# Search for files
> search neural

# Star/unstar important files
> star paper.pdf
> unstar paper.pdf

# List starred files
> starred

# Show file details
> info paper.pdf

# Rename a file
> rename old-name.pdf new-name.pdf

# Delete a file
> delete paper.pdf
```

### Group Management

```bash
# Add existing file to another group
> addto paper.pdf deep-learning

# Remove file from a group
> rmgroup paper.pdf research

# Create parent-child group relationships
> linkgroup deep-learning machine-learning

# Remove parent-child relationship
> unlinkgroup deep-learning machine-learning

# Delete a group
> rmdir old-group
```

### Other Commands

```bash
# Show recent files
> recent

# Show help
> help

# Exit program
> exit
```

## File Structure

```
.
├── DB.c                    # Main source code
├── data/
│   └── database.db        # SQLite database
└── pdf_store/             # Stored files
    ├── paper1.pdf
    ├── paper2.pdf
    └── ...
```

## Database Schema

The system uses four tables:

- **files**: Stores file metadata (name, size, creation date, starred status)
- **groups**: Stores group names
- **file_groups**: Links files to groups (many-to-many relationship)
- **group_hierarchy**: Links groups to parent groups (supports nested groups)

## Examples

### Example Workflow

```bash
# Create a research hierarchy
> mkdir research
> mkdir machine-learning
> mkdir deep-learning
> linkgroup deep-learning machine-learning
> linkgroup machine-learning research

# Add papers
> add ~/Downloads/attention-paper.pdf deep-learning
> add ~/Downloads/transformer-paper.pdf deep-learning
> add ~/Downloads/survey.pdf machine-learning

# Mark important papers
> star attention-paper.pdf

# View hierarchy
> groups

# Search and open
> search attention
> open attention-paper.pdf
```

## Querying the Database Directly

You can also query the SQLite database directly:

```bash
sqlite3 data/database.db
sqlite> .mode column
sqlite> .headers on
sqlite> SELECT * FROM files;
sqlite> SELECT * FROM groups;
sqlite> .exit
```

## Features in Detail

### Hierarchical Groups

Groups can have parent-child relationships, allowing you to create nested organizational structures like:

```
research/
├── machine-learning/
│   ├── deep-learning/
│   │   ├── paper1.pdf
│   │   └── paper2.pdf
│   └── reinforcement-learning/
└── computer-vision/
```

### Multiple Group Membership

A single file can belong to multiple groups simultaneously, useful for cross-cutting concerns:

```
paper.pdf
├── in group: deep-learning
├── in group: computer-vision
└── in group: 2024-papers
```

### Star System

Mark important files with stars for quick access later. Starred files appear at the top of search results.

## Notes

- Files are physically copied to `pdf_store/` directory
- Database is stored in `data/database.db`
- Despite the name, the system works with any file type, not just PDFs
- File deletion removes both the database entry and physical file

## License

This project is open source and available under the MIT License.

## Contributing

Contributions are welcome! Feel free to submit issues or pull requests.

## Future Improvements

- Tag system in addition to groups
- Full-text search within PDF contents
- Export/import functionality
- Web interface
- Automatic paper metadata extraction
- Duplicate detection
