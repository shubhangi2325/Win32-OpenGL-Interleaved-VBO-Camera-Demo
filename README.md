# Win32-OpenGL-Interleaved-VBO-Camera-Demo
This project demonstrates a small modular rendering system built on top of OpenGL and the Win32 API.  
It showcases several fundamental components commonly found in real-time graphics engines.

### Core Components

**Window System**
- Win32 API window creation
- OpenGL context initialization

**Input System**
- Keyboard input (WASD camera movement)
- Mouse movement for camera rotation
- Mouse wheel zoom (FOV control)

**Camera System**
- First-person camera implementation
- Yaw and pitch based rotation
- View matrix calculation

**Rendering System**
- Shader-based rendering pipeline
- Vertex Buffer Objects (VBO)
- Vertex Array Objects (VAO)

**Interleaved Vertex Layout**
Each vertex stores multiple attributes in a single buffer:
Position | Color | Normal | Texture Coordinates

[x y z | r g b | nx ny nz | u v]

**Lighting**
- Basic lighting calculations in GLSL shaders
- Surface normals used for light interaction

**Texture System**
- Texture loading
- Texture coordinate mapping

## Concepts Demonstrated

- Modern OpenGL rendering pipeline
- Interleaved vertex buffer layout
- Shader-based lighting
- Perspective projection
- Camera transformation mathematics
- GPU vertex attribute configuration

## Graphics Architecture

The project follows a simple real-time rendering architecture similar to basic graphics engines.

+-----------------------+
|       Application     |
|   (Main Loop Logic)   |
+-----------+-----------+
            |
            v
+-----------------------+
|     Input System      |
|  Keyboard / Mouse     |
+-----------+-----------+
            |
            v
+-----------------------+
|     Camera System     |
|   View Transformation |
+-----------+-----------+
            |
            v
+-----------------------+
|   Rendering System    |
|  Shader + VBO + VAO   |
+-----------+-----------+
            |
            v
+-----------------------+
|      GPU Pipeline     |
| Vertex → Fragment     |
+-----------+-----------+
            |
            v
+-----------------------+
|     Framebuffer       |
|     Screen Output     |
+-----------------------+
