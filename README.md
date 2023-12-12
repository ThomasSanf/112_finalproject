# Brick Breaker Game

This repository contains the source code for a simple Brick Breaker game developed in C using the SDL2 library. 

## Prerequisites

Before you can run the game, you need to have SDL2 installed on your system. SDL2 is a cross-platform development library designed to provide low-level access to audio, keyboard, mouse, joystick, and graphics hardware via OpenGL and Direct3D.

## Installation

### Installing SDL2

#### For Windows Users

1. **Download SDL2**: Go to the [SDL2 download page](https://www.libsdl.org/download-2.0.php) and download the SDL2 development library for Visual Studio.
2. **Extract the Files**: Extract the downloaded file to a known location on your system (e.g., `C:\\SDL2`).

#### For macOS and Linux Users

macOS and Linux users can install SDL2 via their package managers (like Homebrew on macOS or apt-get on Ubuntu).

### Setting Up SDL2 in Visual Studio

1. **Create a Project**: Start Visual Studio and create a new project for the Brick Breaker game.
2. **Include SDL2 Headers**: 
   - Right-click on your project in the Solution Explorer.
   - Select 'Properties'.
   - Go to `C/C++` > `General` > `Additional Include Directories`.
   - Add the path to the SDL2 headers (e.g., `C:\\SDL2\\include`).
3. **Link SDL2 Libraries**: 
   - In the same Properties window, go to `Linker` > `General` > `Additional Library Directories`.
   - Add the path to the SDL2 libraries (e.g., `C:\\SDL2\\lib\\x64` for 64-bit, `C:\\SDL2\\lib\\x86` for 32-bit).
   - Then go to `Linker` > `Input` > `Additional Dependencies`.
   - Add `SDL2.lib` and `SDL2main.lib`.
4. **Copy SDL2.dll to Your Project**: 
   - Copy `SDL2.dll` from the SDL2 lib directory to your project's executable directory.

## Running the Game

After setting up SDL2 in Visual Studio and building the project, you should be able to run the Brick Breaker game from within Visual Studio.

## Contributing

Contributions to the Brick Breaker game are welcome. Please feel free to fork the repository, make changes, and submit a pull request.
