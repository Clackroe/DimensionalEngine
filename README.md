# Dimensional Engine | 3D Game Engine
This game engine is being developed by myself. I am using it as a way to further my knowledge of low level programming, memory management, inter-working systems, general Engine development, and game Rendering.

## What's unique?
Currently, nothing is too unique about anything I am doing in this repository. However, I would like for it to eventually support a unique system of utilizing 2D sprites in a 3D world. I want to develop a system that can:
- Generate a mesh from a 2D sprite
- Automatically generate a pbr material for the sprite to support being lit "realistically"
- Provide a tool to modify and customize the material simply and efficiently in-editor

I have always liked the look of games that incoorporate 2D sprites/characters into a 3D world. As far as I know, there is not an engine that supports integrating these sprites into a PBR renderer directly. Of course it is possible in other engines, but I would like to provide tools to quickly and easily support this out of the box.

# Current Capabilities
![image](https://github.com/user-attachments/assets/b57261e1-d146-498d-80eb-a918ef313872)


Run the following command to get started
```bash
git clone https://github.com/Clackroe/DimensionalEngine.git --recursive
```

# Development
This project is being developed from scratch (minus a couple of low level libraries) on Linux in Neovim support for Windows is planned to be maintained.

# Build System

## Linux
```bash
mkdir build
cd build
cmake ../
make
```

## Windows
Use Visual Studio CMake integration.
