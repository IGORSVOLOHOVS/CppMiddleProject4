# Install

The analyser core needs no installing: put `include` on your include path, compile the files of `src` but `src/cmd_options.cpp` with your project, and build with C++23. At run time it needs the `tree-sitter` CLI on PATH, with a Python grammar it can find.

To build and test it here:

1. Install mingw g++ 13 or newer, CMake 3.28 or newer, Ninja, and LLVM for `clang-tidy` and `clang-format`.
2. Clone this repository.
3. Run `.\recipe.ps1 test`. The first run downloads GoogleTest, whose archive `CMakeLists.txt` pins by hash.

`recipe.ps1` looks for g++ in `C:\ProgramData\mingw64\mingw64\bin` and for LLVM in `C:\Program Files\LLVM\bin`, and adds those folders to its own PATH.

`.\recipe.ps1 build` compiles the core and both test binaries into `build\`. `.\recipe.ps1 install` does nothing: there is no installer yet.

## The metric tests

`metric_tests` parses the Python files of `src/metric_impl/tests/files` with tree-sitter. With the tree-sitter CLI and its Python grammar installed, `.\recipe.ps1 run` builds it and runs it from `build\`, where CMake copies those files. Without tree-sitter every case of it fails.

## The CLI

`analyser` needs Boost.Program_options and `<print>`, so it needs g++ 14 or newer, and `recipe.ps1` does not build it. With those on the machine:

```powershell
cmake -S . -B build-cli -G Ninja -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=OFF
cmake --build build-cli
```

`ANALYSER_CLI` is on by default and finds Boost through `find_package`. No gate builds this path.

`conanfile.py`, `CMakePresets.json`, `scripts/build_windows.ps1`, `.devcontainer` and `.vscode` are what the course and the old GitHub route left. No gate runs them.
