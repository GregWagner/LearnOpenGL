# Creating a Window (LearnOpenGL 01)

Small CMake-based starter project for learning OpenGL. It creates an 800x600 window,
initializes GLAD, and clears the screen each frame.

## Prerequisites

- CMake 3.16 or newer
- A C++17 compiler (GCC, Clang, or MSVC)
- GLFW 3.3 or newer
- OpenGL development libraries

On Debian/Ubuntu-based systems, install the development packages with:

```sh
sudo apt install build-essential cmake libglfw3-dev libgl-dev
```

GLAD sources are vendored in this repository, so no separate GLAD installation is
required.

## Configure

```sh
cmake -S . -B build
```

For an optimized build, add a build type:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
```

## Build

```sh
cmake --build build
```

## Run

```sh
./build/creating_a_window
```

On Windows, run the generated executable from the build directory instead.

## Expected window behavior

- An 800x600 window titled `LearnOpenGL` opens.
- The window background is cleared to a teal color every frame.
- Resizing the window updates the OpenGL viewport through the framebuffer-size
  callback.
- The application exits cleanly when the window is closed.
- GLAD is initialized with the current GLFW OpenGL context before rendering.

## Editor setup

This project enables `CMAKE_EXPORT_COMPILE_COMMANDS`, so configuring CMake produces
`build/compile_commands.json`. Language servers such as `clangd` and VS Code
IntelliSense can use it to resolve headers such as `glad/glad.h`.

Recommended setup:

- Configure normally:
  ```sh
  cmake -S . -B build
  ```
- Point your editor/language server at `build/compile_commands.json`.
- In VS Code, the CMake Tools extension can manage this automatically.
- With `clangd`, this repository includes a `.clangd` file that adds the
  project's `include` directory, so Vim/Neovim can resolve headers such as
  `glad/glad.h`.

If an editor still reports that `glad.h` cannot be found, make sure it is using
the generated compilation database rather than a default include search path.

- `CMakeLists.txt` - CMake configuration
- `src/main.cpp` - application and window setup
- `src/glad.c` - vendored GLAD loader implementation
- `include/glad/glad.h` - vendored GLAD header
- `include/KHR/khrplatform.h` - vendored KHR platform header
- `README.md` - project documentation
- `TODO.txt` - review findings and possible improvements
