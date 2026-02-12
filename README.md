## Shemodogan

**Shemodogan** is a handy framework for quickly setting up Vulkan-based projects. It's implemented in C, with plans for a Rust FFI later on. This library helps streamline the boring parts like initialization and management, so you can jump straight into development.

## Memory Leak Check

To check for memory leaks in your application, use **Valgrind**:

```bash
valgrind --leak-check=full ./text.bin
```
This helps keep your memory clean and efficient.

## Building Shemodogan

### Standalone Build

If you want to build **Shemodogan** by itself, follow these steps:

1. **Configure the Build**:
   ```bash
   cmake -S . -B build
   ```
   This sets everything up for the build.

2. **Compile the Project**:
   ```bash
   cmake --build build
   ```

### Include as a Subdirectory

To include **Shemodogan** in another CMake project:

1. **Add to CMakeLists.txt** of your main project:
   ```cmake
   add_subdirectory(path/to/shd)
   ```
   Replace `path/to/shd` with the actual path to the **Shemodogan** directory.

2. **Link to Your Target**: In your project's CMakeLists.txt, you can link against the **Shemodogan** library with the following command:
   ```cmake
   target_link_libraries(your_project Shemodogan::SHD)
   ```
   Replace `your_project` with the actual name of your target.

And that’s it! You're ready to start your projects with **Shemodogan**, making your life a bit easier.

## License

This project is licensed under the **MIT License**.

The full license text can be found in [LICENSE.md](LICENSE.md) file.