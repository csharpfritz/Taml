# TAML - Tab Annotated Markup Language

I'm still working on a better structure, so please forgive me if the commits seem a little strange. :-) 
Help and contributions are welcome!

---

## Goals ✅
- Cross-platform (Windows / Linux / macOS)
- Build for 32-bit and 64-bit targets (x86 or ARM)
- Clear, simple build and test instructions (see below)
- Default build produces a static library; optional shared library build
- Build outputs are placed in `libs/` inside the build folder (not tracked by git)

---

## Quick build instructions 🔧

Prerequisites:
- CMake (>= 3.19)
- A C++20-capable compiler (MSVC, clang, or GCC)

Recommended build directory: `build` (do not use `build_local` or other names)

General (all platforms):
```bash
cmake -S . -B build -DTAML_BUILD_SHARED=OFF
cmake --build build --config Release
```

Windows (Visual Studio generator, 32-bit example):
```powershell
cmake -S . -B build -G "Visual Studio 18 2026" -A Win32
cmake --build build --config Release
```
To build x64, pass `-A x64`.

Linux / macOS:
```bash
# Native x86_64 (Linux/macOS) - default compilers
cmake -S . -B build -DTAML_BUILD_SHARED=ON -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release

# Native ARM64 (Apple Silicon / aarch64 Linux) using clang
cmake -S . -B build -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ -DTAML_BUILD_SHARED=ON -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release

# macOS (Xcode generator) for Apple Silicon (ARM64):
# cmake -S . -B build -G "Xcode" -A arm64 -DTAML_BUILD_SHARED=ON
# cmake --build build --config Release
```

To run tests (after building):
```bash
cd build
ctest --output-on-failure -C Release
```

---

## Naming conventions and output
- Library files are placed in `build/libs/` after build (this folder is ignored by Git).
- Output names include platform and bitness. Examples:
  - Windows (static or import lib): `taml(-d for debug)-windows64.lib` and DLL `taml(-d)-windows64.dll`
  - Linux: `libtaml(-d)-linux64.so`
  - macOS: `libtaml(-d)-mac64.so`

Note: the exact suffix ordering may vary slightly depending on CMake configuration; debug builds include `-d`.

---

## Tests
- A simple smoke test executable is included that is built when `-DTAML_BUILD_TESTS=ON`.
- Tests are implemented without third-party frameworks so they work on fresh systems.

---

## Project layout
- `include/` — public headers (`Taml.h` and `Taml/`)
- `src/` — implementation `.cpp` files
- `build/` — generated build files and `libs/` (not committed)
- `tests/` — small smoke tests

---


