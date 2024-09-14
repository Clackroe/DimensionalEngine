# Dimensional Engine | 3D Game Engine

- [What's Unique?](#whats-unique)
- [General Architecture](#general-architecture)
- [Current Capabilities](#current-capabilities)
- [Feature Progress](#feature-progress)
- [Sneak Peak](#sneak-peak)
- [Dimensional API](#dimensional-api)
  - [Input](#input)
  - [Scene](#scene)
- [Libraries](#libraries)
- [Getting Started](#getting-started)
  - [Building](#building)
- [Resources Used for Learning](#resources-used-for-learning)

---
This game engine is being developed by myself, primarily from scratch in terms of rendering. (Minus some [Libraries](#libraries)) I am using it as a way to further my knowledge of low level programming, memory management, inter-working systems, general Engine development, and game Rendering.

## What's unique?
While the engine is still under development, my goal is to support a built in, seamless, system for integrating 2D sprites into a 3D world with PBR rendering. This will enable:
- Automated mesh generation from 2D sprites.
- Built-in PBR material generation with realistic lighting for sprites.
- Editor tools for quick material customization.

I have always liked the look of games that incorporate 2D sprites/characters into a 3D world. As far as I know, there is not an engine that supports integrating these sprites into a PBR renderer directly. Of course it is possible in other engines, but I would like to provide tools to quickly and easily support this out of the box.

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
- Custom [Input](#input) System
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
**PBR Rendering**: This image demonstrates the engine's capability to handle HDR environment maps and materials with normal and roughness maps.
![2024-09-13_16-41](https://github.com/user-attachments/assets/3383a065-49fc-4f1b-8a56-a0d54ab9d036)
**Point and SpotLights**: Showcasing the ability to customize light properties in real-time.
![2024-09-13_16-49_1](https://github.com/user-attachments/assets/3164d96e-617f-4ff5-ab99-911903847f1c)

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

## Libraries

- [**ASSIMP**](https://github.com/assimp/assimp)  
  Portable library to import 3D models into Dimensional's Format.

- [**ENTT**](https://github.com/skypjack/entt)  
  A fast and reliable entity-component system.
  
- [**GLAD**](https://github.com/Dav1dde/glad)  
  For loading OpenGL GPU Driver functions.

- [**GLFW**](https://github.com/glfw/glfw)  
   Provides a simple API for creating windows, contexts, and handling input.

- [**GLM**](https://github.com/g-truc/glm)  
  OpenGL compliant linear algebra math library.

- [**IMGUI**](https://github.com/ocornut/imgui)  
  For creating a functional, fast, and easy to use editor.

- [**STB Image**](https://github.com/nothings/stb)  
  For loading images into memory.

- [**spdlog**](https://github.com/gabime/spdlog)  
  Used behind the scenes for formatted Dimensional Logging.


# Getting Started
Make sure you have [Git Large File Storage](https://git-lfs.com/) installed. If you don't, you will be unable to get the assets needed for the engine.

Run the following command to get the source code
```bash
git clone https://github.com/Clackroe/DimensionalEngine.git --recursive
```

# Building
At this time, only Linux and Windows are supported. 

***NOTE***: Windows Support may be further behind Linux, as I am developing this on Linux. I will do my best to bring the Windows support up to date on every major development.

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

## Usage
- Ensure Portal.exe is your build and launch target.
- Feel free to mess around in PortalLayer.cpp!

## Resources Used For Learning
[Learn OpenGL Book | Online Text Book](https://learnopengl.com/)

[Game Engine Architecture - Jason Gregory | Text Book](https://www.gameenginebook.com/)

[Yan Chernikov (The Cherno) | YouTube](https://www.youtube.com/@TheCherno)

[OGLDEV | YouTube](https://www.youtube.com/@OGLDEV)

Thank you to all of the people and resources listed above! I have learned so much from all of you.
