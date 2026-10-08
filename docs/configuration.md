# Configuration

Everything in `xyo-platform` — and in every library above it — is selected at
compile time with `XYO_PLATFORM_*` macros. This page lists them all.

## How `Config.hpp` is produced

`source/XYO/Platform/Config.hpp` is **generated** and git-ignored. It is the
configuration the library was compiled with, so consumers see the same values
through the installed headers.

1. `fabricare/make.config.js` chooses the defines for the current platform:

   | Condition                  | Defines                                            |
   |----------------------------|----------------------------------------------------|
   | Windows                    | `XYO_PLATFORM_OS_WINDOWS`                          |
   | platform name has `msvc`   | `XYO_PLATFORM_COMPILER_MSVC`                       |
   | platform name has `mingw`  | `XYO_PLATFORM_COMPILER_GCC`, `XYO_PLATFORM_OS_MINGW` |
   | Linux                      | `XYO_PLATFORM_OS_LINUX`, `XYO_PLATFORM_COMPILER_GCC` |
   | always                     | `XYO_PLATFORM_MULTI_THREAD`, `XYO_PLATFORM_NAME="<platform>"` |
   | machine `x64` / `x86`      | `XYO_PLATFORM_64BIT` / `XYO_PLATFORM_32BIT`        |

2. It compiles and runs `source/XYO/Platform.Config.cpp` with those defines.
   The program copies `Config.Template.hpp`, uncommenting each
   `// #define XYO_PLATFORM_...` line whose macro is defined, and writes
   `Config.hpp` only if the content changed (so a rebuild does not touch every
   file).

Generated example (win64-msvc-2026):

```cpp
// -- Operating System
#define XYO_PLATFORM_OS_WINDOWS
// #define XYO_PLATFORM_OS_LINUX
// #define XYO_PLATFORM_OS_MINGW
// #define XYO_PLATFORM_OS_EMSCRIPTEN

// -- Compiler
#define XYO_PLATFORM_COMPILER_MSVC
// #define XYO_PLATFORM_COMPILER_GCC

// -- Architecture
#define XYO_PLATFORM_64BIT
// #define XYO_PLATFORM_32BIT

// -- Thread support
// #define XYO_PLATFORM_SINGLE_THREAD
#define XYO_PLATFORM_MULTI_THREAD

// -- Platform
#define XYO_PLATFORM_NAME "win64-msvc-2026"
```

To bypass the file completely, define `XYO_PLATFORM_CONFIG_HPP` and pass all
the macros on the compiler command line: `Dependency.hpp` includes
`Config.hpp` only if `XYO_PLATFORM_CONFIG_HPP` is not yet defined.

> A Linux (WSL) build and a Windows build of the same checkout overwrite each
> other's `Config.hpp`. Rebuild on the platform you are working on before
> using the IDE or compiling by hand.

## Platform macros

| Macro | Meaning |
|-------|---------|
| `XYO_PLATFORM_OS_WINDOWS` | Windows API (`windows.h`). Also set by `_OS_MINGW`. |
| `XYO_PLATFORM_OS_LINUX` | POSIX / pthreads. Also set by `_OS_EMSCRIPTEN`. |
| `XYO_PLATFORM_OS_MINGW` | MinGW toolchain on Windows. Implies `XYO_PLATFORM_OS_WINDOWS`. |
| `XYO_PLATFORM_OS_EMSCRIPTEN` | WebAssembly via Emscripten. Implies `XYO_PLATFORM_OS_LINUX`. |
| `XYO_PLATFORM_COMPILER_MSVC` | Microsoft C++ compiler. |
| `XYO_PLATFORM_COMPILER_GCC` | GCC or a GCC compatible compiler (Clang, MinGW, emcc). |
| `XYO_PLATFORM_64BIT` / `XYO_PLATFORM_32BIT` | Target pointer size. Default: 64 bit. |
| `XYO_PLATFORM_MULTI_THREAD` / `XYO_PLATFORM_SINGLE_THREAD` | Thread support. Default: multi thread. |
| `XYO_PLATFORM_NAME` | String literal, the fabricare platform name, e.g. `"win64-msvc-2026"`, `"ubuntu-26.04"`. |

Defaults and checks applied by `Dependency.hpp`:

- neither `_MULTI_THREAD` nor `_SINGLE_THREAD` → `XYO_PLATFORM_MULTI_THREAD`;
- neither `_64BIT` nor `_32BIT` → `XYO_PLATFORM_64BIT`;
- no compiler macro → `#error`;
- no OS macro → `#error`.

Test the **family** in your code, not the variant:

```cpp
#ifdef XYO_PLATFORM_OS_WINDOWS   // true for MSVC and MinGW
#endif
#ifdef XYO_PLATFORM_OS_LINUX     // true for Linux and Emscripten
#endif
#ifdef XYO_PLATFORM_OS_MINGW     // only when the MinGW difference matters
#endif
```

### Single thread mode

With `XYO_PLATFORM_SINGLE_THREAD` the library keeps the same names but they
cost nothing:

| Name | Multi thread | Single thread |
|------|--------------|---------------|
| `TAtomic<T>` | wraps `std::atomic<T>` | wraps a plain `T`, memory orders ignored, `isAtomic == false` |
| `CriticalSection` | OS mutex | empty inline `enter()` / `leave()`, `isCriticalSection == false` |
| `ConditionVariable` | OS condition variable | **not declared** |
| `Thread` | class | **not declared**; only the function `Thread::sleep(int)` (a namespace) |
| `XYO_PLATFORM_THREAD_LOCAL` | `__declspec(thread)` / `__thread` | empty |
| `Processor::getCount()` | available | available |

Code that must compile in both modes should use only `TAtomic`,
`CriticalSection`, `Thread::sleep`, `Processor::getCount` unconditionally and
guard the rest with `#ifdef XYO_PLATFORM_MULTI_THREAD`.

## Export / linking macros

Every exported function or class member is marked `XYO_PLATFORM_EXPORT`.
Its value depends on how the code is being compiled:

| Defined by the build | `XYO_PLATFORM_EXPORT` becomes | Situation |
|----------------------|-------------------------------|-----------|
| `XYO_PLATFORM_DLL_INTERNAL` | `__declspec(dllexport)` (MSVC) | compiling project `xyo-platform` as DLL |
| *(nothing)* | `__declspec(dllimport)` (MSVC) | using the DLL |
| `XYO_PLATFORM_LIBRARY` | empty | using the static library / amalgam |
| `XYO_PLATFORM_LIB_INTERNAL` | empty | compiling project `xyo-platform` as static library |
| `XYO_PLATFORM_STATIC_LIB_INTERNAL` | empty | compiling project `xyo-platform.static` |
| `XYO_PLATFORM_COMPILE_STATIC` | empty | force static for everything |

The `*_INTERNAL` defines are added automatically by the `xyo-cc` compiler
driver from the project name and kind (`<PROJECT>_DLL_INTERNAL`,
`<PROJECT>_LIB_INTERNAL`); `XYO_PLATFORM_LIBRARY` reaches consumers of
`xyo-platform.static` through its `dependencyDefines`.

With GCC all variants are empty (default symbol visibility). The two helper
macros `XYO_PLATFORM_LIBRARY_EXPORT` / `XYO_PLATFORM_LIBRARY_IMPORT` hold the
compiler-specific keywords and can be predefined to override them.

Libraries above follow the same pattern with their own prefix
(`XYO_MANAGEDMEMORY_EXPORT`, `XYO_MULTITHREADING_EXPORT`, ...), so the
`XYO_PLATFORM_LIBRARY_EXPORT/IMPORT` keywords are reused by all of them.

## Helper macros

### `XYO_PLATFORM_DISALLOW_COPY_ASSIGN_MOVE(T)`

Deletes copy/move construction and assignment. It **switches to `private:`**,
so put it first in the class body and write the access specifier you need
after it:

```cpp
class Resource {
		XYO_PLATFORM_DISALLOW_COPY_ASSIGN_MOVE(Resource);

	public:
		Resource();
		~Resource();
};
```

`CriticalSection`, `ConditionVariable` and `Thread` all use it: they can
not be copied, moved or stored by value in containers that move elements.
Hold them as members or behind a pointer.

### `XYO_PLATFORM_INTERFACE(T)`

For abstract interfaces: protected defaulted constructor and destructor plus
`DISALLOW_COPY_ASSIGN_MOVE`. It also leaves the section `private:`.

```cpp
class IStream {
		XYO_PLATFORM_INTERFACE(IStream);

	public:
		virtual size_t read(void *output, size_t length) = 0;
};
```

### `XYO_PLATFORM_THREAD_LOCAL`

Storage class for per-thread variables of trivial types (it is
`__declspec(thread)` / `__thread`, not `thread_local`, so no constructors or
destructors):

```cpp
static XYO_PLATFORM_THREAD_LOCAL int depth = 0;
```

### `XYO_PLATFORM_FORMAT_SIZET`

`printf` format for `size_t` on the current compiler/platform:

```cpp
printf("size: " XYO_PLATFORM_FORMAT_SIZET "\n", sizeof(buffer));
```

## Other switches

| Macro | Effect |
|-------|--------|
| `XYO_PLATFORM_MEMORY_LEAK_DETECTOR_VLD` | Windows only: includes `windows.h` and `vld.h` (Visual Leak Detector) from `Dependency.hpp`. |
| `XYO_PLATFORM_NO_VERSION` | `Version.rh` reports `0.0.0.0` instead of the real version (reproducible builds). |
