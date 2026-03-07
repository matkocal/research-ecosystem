# Launcher

The main entry point for research_ecosystem. Written in Perl.

## What it does

Runs the Ada integrity check first on every startup. If the check passes, presents a menu to launch any component of the ecosystem. Finds all apps by their path relative to its own location, so it works correctly from any machine, any USB mount point, without any configuration.

## How to run
```bash
perl launcher.pl
```

## Menu options

1. DB - launches the C-based file and knowledge database
2. Photos - launches the Perl photo organizer
3. Contacts - launches the Perl contact manager
4. Radio - launches the Go packet radio communication app
5. Exit - closes the launcher

## Portability

The launcher calculates its own location at runtime using abs_path. No hardcoded paths anywhere. Copy the whole ecosystem folder to any USB or any machine and it works immediately.

## Part of research_ecosystem

This is the glue layer of research_ecosystem, a fully offline personal cloud designed to run from a USB drive with no internet dependency.
