# curlser

A CLI tool written in C that performs HTTP requests, but with automatic output formatting based on Content-Type.

## Features

- [x] HTTP requests via libcurl
- [x] Automatic Content-Type detection
- [x] JSON formatting with syntax highlighting
- [x] XML formatting with syntax highlighting
- [x] HTML formatting with syntax highlighting
- [x] HTTP methods support (GET, POST, PUT, DELETE, etc)
- [x] Custom headers support
- [x] Request body support

## Installation

### macOS (Homebrew)

```bash
brew tap kaiopiola/curlser https://github.com/kaiopiola/curlser
brew install curlser
```

Or install directly without tap:

```bash
brew install --formula https://raw.githubusercontent.com/kaiopiola/curlser/v1.0.0/Formula/curlser.rb
```

### Manual Installation

See [INSTALL.md](INSTALL.md) for detailed instructions.

## Dependencies

- libcurl (for HTTP requests)
- C compiler (gcc, clang, or MSVC)

### Installing Dependencies

```bash
# macOS (Homebrew)
brew install curl

# Ubuntu/Debian
sudo apt-get install libcurl4-openssl-dev

# Fedora/RHEL
sudo dnf install libcurl-devel

# Windows (vcpkg)
vcpkg install curl
```

## Building

```bash
# Linux/macOS
make

# Windows (with MinGW)
mingw32-make

# Or directly
gcc -o curlser src/main.c src/http.c src/formatters/*.c -lcurl
```

## Usage

```bash
# Simple GET request
./bin/curlser https://api.example.com/data

# POST request with JSON
./bin/curlser -X POST -H "Content-Type: application/json" -d '{"name": "test"}' https://api.example.com/create

# Show response headers
./bin/curlser -i https://api.example.com/data

# Raw output (no formatting)
./bin/curlser -r https://api.example.com/data

# Verbose mode
./bin/curlser -v https://api.example.com/data
```

## Options

| Option | Description |
|--------|-------------|
| `-X, --request` | HTTP method (GET, POST, PUT, DELETE, etc) |
| `-H, --header` | Custom header (can be used multiple times) |
| `-d, --data` | Data to send in request body |
| `-i, --include` | Include response headers in output |
| `-r, --raw` | Raw output, no formatting |
| `-v, --verbose` | Verbose mode |
| `-h, --help` | Show help |
| `-V, --version` | Show version |

## Project Structure

```
curlser/
├── src/
│   ├── main.c              # Entry point
│   ├── http.c              # HTTP request functions
│   ├── http.h
│   ├── colors.h            # ANSI color definitions
│   └── formatters/
│       ├── formatters.c    # Content-Type detection
│       ├── formatters.h
│       ├── json.c          # JSON formatter
│       ├── xml.c           # XML formatter
│       └── html.c          # HTML formatter
├── build/                  # Object files (generated)
├── bin/                    # Executable (generated)
├── Makefile
└── README.MD
```

## Terminal Colors

The tool uses ANSI codes to colorize output:

- **JSON**:
  - Braces/brackets in bold white
  - Strings in green
  - Numbers in yellow
  - Booleans in magenta
  - null in dim

- **XML**:
  - Tags in blue
  - Tag names in cyan
  - Attributes in yellow
  - Values in green
  - Comments in dim

- **HTML**:
  - Tags in blue
  - Tag names in cyan
  - Attributes in yellow
  - Values in green
  - Content in white

## Output Example

```bash
$ ./bin/curlser https://jsonplaceholder.typicode.com/posts/1

HTTP Status: 200

{
  "userId": 1,
  "id": 1,
  "title": "sunt aut facere repellat...",
  "body": "quia et suscipit..."
}
```

## License

MIT
