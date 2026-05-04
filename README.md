# Lumina Viewer 🎨

Lumina Viewer is a C++ application built using the **Qt** framework and **OpenGL**. It serves as a shader visualization platform for 3D models and geometries (spheres) using custom GLSL shaders.

The project allows exploring different rendering techniques, ranging from traditional OBJ model rasterization to sphere rendering (likely via Raymarching or Raytracing in the fragment shader).

---

## 📂 Project Structure

### 🏢 Engine Core
- **`Scene`**: The main class that manages the overall scene. It holds model instances, loaded resources, and the camera.
- **`ModelResource`**: Manages the loading of a specific OBJ model, its textures, and shaders. It is responsible for creating OpenGL VAOs and VBOs.
- **`ModelInstance`**: Represents an instance of a model within the scene. It stores its transformation matrix (`TG`) and state (e.g., whether textures or bump mapping are active).
- **`Camera`**: A 3D camera implementation supporting movement (WASD), rotation, and orbiting.
- **`model.cpp/h`**: A utility for parsing and loading `.obj` files.

### 🖥️ User Interface (Qt)
- **`Widget` (Main)**: The primary entry window from which different viewers can be opened.
- **`WidgetSpheres` / `WidgetModels` / `WidgetScene`**: Secondary windows containing UI controls (sliders for light, color, etc.) and their respective OpenGL widgets.
- **`MyGLWidget...`**: Classes inheriting from `QOpenGLWidget`. This is where the OpenGL initialization (`initializeGL`), rendering loop (`paintGL`), and keyboard/mouse event handling reside.

### 🎭 Shaders
- **Shaders (`.vert`, `.frag`)**: GLSL code files defining vertex and pixel processing.
  - `vertexModel.vert` / `fragmentModel.frag`: Standard shaders for models with support for lighting, textures, and bump mapping.
  - `fragmentSphere.frag`: A specialized shader for sphere rendering.

---

## ⚙️ How It Works

1. **Entry Point**: The application starts in `main.cpp`, which initializes the main `Widget`.
2. **Mode Selection**: The user chooses from three modes:
   - **Sphere Viewer**: Uses `MyGLWidgetSpheres` to render spheres by passing their data directly to the shader.
   - **Model Viewer**: Allows loading an individual `.obj` file.
   - **Scene Viewer**: Loads a predefined scene via `Scene::loadScene()`, featuring multiple models (trees, cats, etc.).
3. **Rendering Loop**:
   - Each `MyGLWidget` calls `scene.render()`.
   - `Scene` iterates through its `ModelInstance` objects and calls their `render()` method.
   - `ModelInstance` uses its associated `ModelResource` to bind OpenGL shaders and buffers, passing transformation matrices and lighting parameters.

---

## 🛠️ How to Call and Use Each Component

### 1. Loading a Model
To load a model into a scene:
```cpp
scene.loadModel("path/to/model.obj");
```
This automatically creates a `ModelResource` (if it doesn't exist) and a `ModelInstance` centered in the view.

### 2. Lighting Control
The OpenGL widgets expose slots that can be connected to the UI or called manually:
- `setLightPosX(int x)`, `setLightPosY(int y)`, `setLightPosZ(int z)`
- `setLightColorR(int r)`, `setLightColorG(int g)`, `setLightColorB(int b)`

### 3. Activating Effects
Textures and bump mapping can be toggled in real-time:
```cpp
myGLWidget->activeTexture(true);
myGLWidget->activeBump(true);
```

### 4. Camera Controls (In Scene/Model Viewer)
- **W / S**: Move forward / backward.
- **A / D**: Rotate camera.
- **Q / E**: Move up / down.
- **Arrow Keys**: Orbit around the center.
- **R**: Reset camera.