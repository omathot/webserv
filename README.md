# Project Title

## Description
This project is a simple web server written in C++ with only the standard C++ libraries. It handles requests reliably, using `poll()` for event-driven I/O, and includes support for Python CGI scripts and autoindexing functionality for directory listings.

## Requirements
- **Compiler**: C++ with C++98 standard support
- **Flags**: `-Wall -Wextra -Werror`
- **Libraries**: Only standard C++ libraries (no external or Boost libraries)

## Installation
A `Makefile` is provided with the following commands:
- `make` or `make all`: Compile the project
- `make clean`: Remove object files
- `make fclean`: Remove all build files
- `make re`: Rebuild the project

## Usage
After compiling, start the server with:

- Change the root path in config/ of the config file used.
- Run these command without arg to use ourDefault.conf or with the path to desired config file

```bash
./webserver
```

```bash
./webserver config/tempcong.conf
```