# Installation Guide

This guide will help you install curlser so you can use it from anywhere in your terminal by just typing `curlser`.

## Prerequisites

Make sure you have libcurl installed:

```bash
# macOS (Homebrew)
brew install curl

# Ubuntu/Debian
sudo apt-get install libcurl4-openssl-dev

# Fedora/RHEL
sudo dnf install libcurl-devel

# Arch Linux
sudo pacman -S curl

# Windows (vcpkg)
vcpkg install curl
```

## Building

```bash
# Clone or download the project
cd curlser

# Build
make
```

## Installing Globally

### Linux / macOS

**Option 1: Install to /usr/local/bin (recommended)**

```bash
sudo make install
```

This copies the binary to `/usr/local/bin/curlser`.

**Option 2: Manual installation**

```bash
sudo cp bin/curlser /usr/local/bin/
sudo chmod +x /usr/local/bin/curlser
```

**Option 3: Install to user's bin folder (no sudo required)**

```bash
# Create ~/bin if it doesn't exist
mkdir -p ~/bin

# Copy the binary
cp bin/curlser ~/bin/

# Add to PATH (add this to your ~/.bashrc, ~/.zshrc, or ~/.profile)
export PATH="$HOME/bin:$PATH"

# Reload shell config
source ~/.bashrc  # or ~/.zshrc
```

### Windows

**Option 1: Add to PATH**

1. Build the project with MinGW: `mingw32-make`
2. Copy `bin\curlser.exe` to a folder of your choice, e.g., `C:\Tools\`
3. Add the folder to your PATH:
   - Right-click "This PC" > Properties
   - Advanced system settings > Environment Variables
   - Under "User variables", select "Path" and click "Edit"
   - Click "New" and add `C:\Tools\`
   - Click OK to save

**Option 2: Using PowerShell profile**

Add an alias to your PowerShell profile:

```powershell
# Open profile
notepad $PROFILE

# Add this line (adjust path as needed)
Set-Alias curlser "C:\path\to\curlser.exe"
```

## Verifying Installation

After installation, open a new terminal and run:

```bash
curlser --version
```

You should see:

```
curlser version 1.0.0
A CLI tool for HTTP requests with automatic formatting
```

## Uninstalling

### Linux / macOS

```bash
# If installed with make install
sudo make uninstall

# Or manually
sudo rm /usr/local/bin/curlser

# If installed to ~/bin
rm ~/bin/curlser
```

### Windows

Simply delete the `curlser.exe` file and remove the PATH entry if you added one.

## Quick Test

```bash
# Test with a JSON API
curlser https://jsonplaceholder.typicode.com/posts/1

# Test with headers
curlser -i https://jsonplaceholder.typicode.com/posts/1

# Test POST request
curlser -X POST -H "Content-Type: application/json" -d '{"title":"test"}' https://jsonplaceholder.typicode.com/posts
```

## Troubleshooting

### "command not found" after installation

Make sure the installation directory is in your PATH:

```bash
# Check if /usr/local/bin is in PATH
echo $PATH | grep -o '/usr/local/bin'

# If not, add it to your shell config
echo 'export PATH="/usr/local/bin:$PATH"' >> ~/.bashrc
source ~/.bashrc
```

### "libcurl not found" error

Make sure libcurl is installed and the linker can find it:

```bash
# macOS
brew install curl

# Linux
sudo apt-get install libcurl4-openssl-dev  # Debian/Ubuntu
sudo dnf install libcurl-devel             # Fedora/RHEL
```

### Permission denied

```bash
# Make the binary executable
chmod +x /usr/local/bin/curlser
```
