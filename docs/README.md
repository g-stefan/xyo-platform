# XYO Platform — Documentation

`xyo-platform` is the lowest layer of the XYO C++ library stack. It gives the
libraries above it (`xyo-managed-memory`, `xyo-data-structures`,
`xyo-multithreading`, `xyo-system`, `quantum-script`, ...) **one portable
vocabulary** for:

- **what we are compiling for**: operating system, compiler, 32/64 bit,
  single or multi thread, platform name (`XYO_PLATFORM_*` macros);
- **how symbols are exported** from a DLL / shared library or linked
  statically (`XYO_PLATFORM_EXPORT`);
- **the minimal multithreading primitives** every other library needs:
  atomics, a mutex, a condition variable, threads and the processor count.

It deliberately contains nothing else. Higher level features (thread pools,
workers, memory management, containers, files, sockets) live in the libraries
that depend on it.

```
quantum-script, applications ...
xyo-system, xyo-encoding, xyo-cryptography, ...
xyo-multithreading, xyo-data-structures
xyo-managed-memory
xyo-platform          <-- this library (C++ standard library + OS API only)
```

## Why a separate platform layer

- **One place for `#ifdef`s.** Code above uses `XYO_PLATFORM_OS_WINDOWS`,
  `XYO_PLATFORM_MULTI_THREAD`, ... instead of testing `_WIN32`, `__GNUC__`,
  `_MSC_VER` itself.
- **Single thread builds.** The same source compiles with
  `XYO_PLATFORM_SINGLE_THREAD`; the primitives then become zero-cost
  (`CriticalSection` is empty, `TAtomic<T>` is a plain `T`).
- **Hidden OS headers.** Public headers never include `<windows.h>` or
  `<pthread.h>`; the OS objects live behind an opaque pointer (pimpl) in the
  `-OS-Windows.cpp` / `-OS-Linux.cpp` files.
- **Stable ABI across DLL boundaries.** Classes do not expose OS types, so
  the DLL can change its implementation without recompiling users.

## Supported targets

| Target                          | Defines                                                 |
|---------------------------------|---------------------------------------------------------|
| Windows, MSVC                   | `XYO_PLATFORM_OS_WINDOWS`, `XYO_PLATFORM_COMPILER_MSVC` |
| Windows, MinGW                  | `XYO_PLATFORM_OS_MINGW` (implies `_OS_WINDOWS`), `XYO_PLATFORM_COMPILER_GCC` |
| Linux (GCC / Clang)             | `XYO_PLATFORM_OS_LINUX`, `XYO_PLATFORM_COMPILER_GCC`    |
| Emscripten                      | `XYO_PLATFORM_OS_EMSCRIPTEN` (implies `_OS_LINUX`), `XYO_PLATFORM_COMPILER_GCC` |

Requires C++17 (nested namespace definitions, `std::atomic<T>::is_always_lock_free`).

## Contents

| Document | What it covers |
|----------|----------------|
| [Getting started](getting-started.md) | Build it, depend on it (DLL or static), first program |
| [Configuration](configuration.md) | `Config.hpp`, all `XYO_PLATFORM_*` macros, export macros, helper macros |
| [Multithreading](multithreading.md) | `TAtomic`, `CriticalSection`, `ConditionVariable`, `Thread`, `Processor` — usage and pitfalls |
| [API reference](reference.md) | Every public symbol on one page |

## Source map

```
source/XYO/Platform.hpp                  umbrella header, include this
source/XYO/Platform.Amalgam.cpp          whole library in one translation unit
source/XYO/Platform.Config.cpp           generator for Config.hpp (build time)
source/XYO/Platform/
    Config.Template.hpp                  template, Config.hpp is generated from it
    Dependency.hpp                       std headers, defaults, checks, export macros
    Type.hpp                             XYO_PLATFORM_DISALLOW_COPY_ASSIGN_MOVE / _INTERFACE
    Copyright / License / Version        library metadata
    Multithreading/
        TAtomic.hpp                      header only
        CriticalSection[-OS-*.cpp]
        ConditionVariable[-OS-*.cpp]
        Thread[-OS-*.cpp]
        Processor[-OS-*.cpp]
test/test.01.cpp                         prints the platform name
test/test.02.cpp                         ConditionVariable behaviour tests
```

## AI assistant skill

A Claude Code skill describing how to use this library lives in
[`.claude/skills/xyo-platform/`](../.claude/skills/xyo-platform/SKILL.md).
It is picked up automatically inside this repository; copy the folder to
`~/.claude/skills/` to have it available in the projects that depend on
`xyo-platform`.
