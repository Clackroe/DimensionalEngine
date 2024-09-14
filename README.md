[Getting Started](#getting-started)
[Dimensional API Examples](#dimensional-api)

# Dimensional Engine | 3D Game Engine
This game engine is being developed by myself. I am using it as a way to further my knowledge of low level programming, memory management, inter-working systems, general Engine development, and game Rendering.

## What's unique?
Currently, nothing is too unique about anything I am doing in this repository. However, I would like for it to eventually support a unique system of utilizing 2D sprites in a 3D world. I want to develop a system that can:
- Generate a mesh from a 2D sprite
- Automatically generate a pbr material for the sprite to support being lit "realistically"
- Provide a tool to modify and customize the material simply and efficiently in-editor

I have always liked the look of games that incoorporate 2D sprites/characters into a 3D world. As far as I know, there is not an engine that supports integrating these sprites into a PBR renderer directly. Of course it is possible in other engines, but I would like to provide tools to quickly and easily support this out of the box.

# General Architecture
Dimensional Engine is currently split into two parts. 
- Core Runtime
  - A shared/dynamic library  responsible for all runtime specific code like:
    - Rendering
    - Event Handling
    - Entity Handling
    - Asset Management
    - Etc
- Portal (Dimensional's Editor)
  - An executable responsible for giving the User an interface to edit scenes and projects
  - Linked to the Core Runtime shared/dynamic library
  - Manages:
    - Active Scenes
    - Scene State
    - Entity State During Game Development
    - Developer Tools and Panels

Eventually there will be another executable, the Application Build, that will be similar to to the editor, but optimized for runtime speed as opposed to development. 

# Current Capabilities

- PBR Rendering with HDR environment maps:
  - Cubemaps, Prefiltered Maps, and Irradiance maps are all precalculated and generated using Compute shaders
  - Point and Spot Lights that support in-editor customization of attenuation settings, color, and transform
  - WIP Model and Mesh System using [ASSIMP](https://github.com/assimp/assimp)
  - PBR Materials that support:
    - NormalMaps
    - AO Maps
    - Albedo Maps
    - Metalness Maps
    - Roughness Maps
- Entity and Scene System with custom components
  - Implemented behind the scenes with [ENTT](https://github.com/skypjack/entt)
- Custom Event System
  - Easily add event listeners to Core and/or Custom events
  - Functions provided to the listeners are executed at the end of each frame.
- Custom Input System
  - Input Events can be listened to and executed through Dimensional's event system
  - OR queried in the game loop using IsKeyDown. (This part is implemented with the window Library, [GLFW](https://www.glfw.org/) behind the scenes)
 
## Feature Progress

| **Feature**                  | **Status**          | **Notes**                                                                 |
|------------------------------|---------------------|---------------------------------------------------------------------------|
| PBR Rendering                 | Completed           | Includes HDR environment maps, normal maps, AO maps, etc.                 |
| Entity and Scene System       | In Progress         | Powered by ENTT, custom components implemented.                           |
| Serialization and Deserialization| In Progress         | Development saving and loading of Assets, scenes, and Projects using YAML                           |
| Event System                  | Completed           | Custom event system for core and custom events.                           |
| Input System                  | Completed           | Supports event-driven and query-based input.                  |
| 2D Sprite to 3D Mesh System   | Planned             | Plan to generate a 3D mesh from a 2D sprite with auto PBR material setup and editing. |
| Customizable PBR Material for 2D | Planned             | Develop editor tools for quick material customization.                    |
| Editor Improvements           | In Progress            | Expand editor interface for improved scene and entity management.         |
| Shadow Mapping                | Planned             | Add support for shadow mapping with point, spot, and directional lights.                |
## Sneak Peak
![2024-09-13_16-29_1](https://github.com/user-attachments/assets/69815ff6-9ace-48c0-adb4-b2343bcc1330)
*PBR Rendering*
![2024-09-13_16-41](https://github.com/user-attachments/assets/3383a065-49fc-4f1b-8a56-a0d54ab9d036)
*Point and SpotLights*
![2024-09-13_16-49_1](https://github.com/user-attachments/assets/3164d96e-617f-4ff5-ab99-911903847f1c)
*Entity Properties Panel*

# Dimensional API
## Input
*Query Input*
```c++
    if (Input::IsKeyDown(Key::W)) {
        m_Position += forward * cameraSpeed;
    }
```
*Event Driven*
```c++
    EventSystem::AddListener<MouseEvent>([](const Ref<MouseEvent>& e) {
        float x = e->getX();
        float y = e->getY();
        m_MouseX = x;
        m_MouseY = y;
    });
```
## Scene
*Add and Manipulate Entities*
```c++
        auto ent3 = m_ActiveScene->createEntity("PointLight");
        ent3.addComponent<PointLightComponent>();
        auto& t1 = ent3.getComponent<TransformComponent>();
        auto& l = ent3.getComponent<PointLightComponent>();
        l.intensity = 0.1;
        l.constant = 8;
        l.linear = 0.09;
        l.quadratic = 0.332;
        t1.Scale = glm::vec3(0.1);
        t1.Position = { 0.0f, -1.5f, 13.0f };
```

# Getting Started
Make sure you have [Git Large File Storage](https://git-lfs.com/) installed. If you don't, you will be unable to get the assets needed for the engine.

Run the following command to get the source code
```bash
git clone https://github.com/Clackroe/DimensionalEngine.git --recursive
```

# Building
This project uses CMake. Ensure you have both CMake and Make for linux or Visual Studio for Windows.
## Linux
```bash
mkdir build
cd build
cmake ../
make
```

## Windows
Use Visual Studio CMake integration.
