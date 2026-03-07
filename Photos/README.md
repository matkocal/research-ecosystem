# 📷 Perl Photo Gallery

A simple command-line photo management app written in Perl. Organize, browse, and manage your photo files right from the terminal.

## Features

- Add photos to a local gallery folder
- List all photos in your gallery
- Open photos with your system's default viewer
- Delete photos
- Rename photos
- Star your favorite photos and list them

## Requirements

- Perl 5 (with `use strict` and `use warnings`)
- The `File::Copy` core module (included with Perl)
- A system image viewer (`open` on macOS, `xdg-open` on Linux)

## Getting Started

Clone the repository and run the script:

```bash
git clone https://github.com/yourusername/perl-photo-gallery.git
cd perl-photo-gallery
perl gallery.pl
```

## Usage

When you launch the app, you'll see a menu:

```
---OPTIONS---
0. Add a file
1. List all files
2. Open a file
3. Delete a file
4. Rename a file
5. Star a file
6. List starred files
7. Exit
```

Enter the number corresponding to the action you want to perform and follow the prompts.

### Adding a Photo

Choose option `0` and enter the path to a file on your system. It will be copied into the `photos/` folder.

### Opening a Photo

Choose option `2` and enter the filename. The app will open it using your system's default application.

### Starring Photos

Use option `5` to star a file and `6` to view your starred list. Note: starred files are stored in memory and will reset when the app closes.

## File Storage

All photos are stored in a `photos/` directory created automatically in the same folder as the script.

## Notes

- The `photos/` directory is created automatically on first run if it doesn't exist.
- Starred photos are not persisted between sessions — stars will reset when the app closes.
- When opening a file (option 2), make sure to enter the full path or filename as stored in the gallery.

## License

MIT
