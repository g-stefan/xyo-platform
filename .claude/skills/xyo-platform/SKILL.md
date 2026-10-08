---
name: xyo-platform
description: >-
  How to use the xyo-platform C++ library (namespace XYO::Platform), the base
  layer of the XYO C++ stack: XYO_PLATFORM_* configuration macros (OS, compiler,
  32/64 bit, single/multi thread), XYO_PLATFORM_EXPORT DLL export macros,
  XYO_PLATFORM_DISALLOW_COPY_ASSIGN_MOVE / XYO_PLATFORM_INTERFACE, and the
  multithreading primitives TAtomic, CriticalSection, ConditionVariable, Thread
  and Processor::getCount. Use when writing or reviewing code that includes
  <XYO/Platform.hpp>, depends on "xyo-platform" in fabricare.json, uses any of
  these names, or when working inside the xyo-platform repository or libraries
  built on it (xyo-managed-memory, xyo-data-structures, xyo-multithreading,
  xyo-system, quantum-script).
---

# xyo-platform

Lowest layer of the XYO C++ libraries. It gives everything above it one
vocabulary for **what we compile for** (macros), **how symbols are exported**
(DLL vs static), and **minimal threading primitives**. Nothing else: thread
pools, lock guards, containers, memory management live in higher libraries.

Full documentation: `docs/` in the xyo-platform repository
(`X:\Storage\XYO\Gitea\CPP\xyo-platform\docs` on this machine) — README,
getting-started, configuration, multithreading, reference. Read the matching
page when you need more than this summary. Source is small; when in doubt read
the header in `source/XYO/Platform/`.

## Using it

- Include `#include <XYO/Platform.hpp>` (umbrella). C++17.
- fabricare consumer: `"dependency": ["xyo-platform"]` (DLL) or
  `["xyo-platform.static"]` (static; exports `XYO_PLATFORM_LIBRARY` to the
  consumer). Build/install with fabricare (see the `fabricare` skill); install
  xyo-platform to the SDK before building dependents.
- Without fabricare: compile `source/XYO/Platform.Amalgam.cpp` with
  `-DXYO_PLATFORM_LIBRARY`, `source/` on the include path, a
  `source/XYO/Platform/Config.hpp` present, `-pthread` on Linux.
- Libraries above re-export names in their own `Dependency.hpp`:
  `using XYO::Platform::Multithreading::TAtomic;` etc.

## Configuration macros

`source/XYO/Platform/Config.hpp` is **generated** (git-ignored) by
`fabricare/make.config.js` → compiles/runs `Platform.Config.cpp`, which
uncomments lines of `Config.Template.hpp`. Never commit or hand-edit it in the
repo; a WSL build overwrites the Windows one (rebuild before using the IDE).

- OS: `XYO_PLATFORM_OS_WINDOWS` (also set by `_OS_MINGW`),
  `XYO_PLATFORM_OS_LINUX` (also set by `_OS_EMSCRIPTEN`). Test the family
  (`_WINDOWS` / `_LINUX`), not `_WIN32` / `__linux__`.
- Compiler: `XYO_PLATFORM_COMPILER_MSVC` / `XYO_PLATFORM_COMPILER_GCC`
  (GCC, Clang, MinGW, emcc).
- `XYO_PLATFORM_64BIT` / `_32BIT` (default 64), `XYO_PLATFORM_MULTI_THREAD` /
  `_SINGLE_THREAD` (default multi), `XYO_PLATFORM_NAME` (string, e.g.
  `"win64-msvc-2026"`).
- Missing OS or compiler macro → `#error` in `Dependency.hpp`.

Helpers:

- `XYO_PLATFORM_EXPORT` on every exported function/member. `xyo-cc` defines
  `XYO_PLATFORM_DLL_INTERNAL` (→ dllexport) or `_LIB_INTERNAL` /
  `_STATIC_LIB_INTERNAL` (→ empty); consumers get dllimport unless
  `XYO_PLATFORM_LIBRARY` is defined. New XYO libraries copy this pattern with
  their own prefix and reuse `XYO_PLATFORM_LIBRARY_EXPORT/IMPORT`.
- `XYO_PLATFORM_DISALLOW_COPY_ASSIGN_MOVE(T)` — deletes copy/move; it ends in
  `private:`, so put it first in the class and write `public:` after it.
- `XYO_PLATFORM_INTERFACE(T)` — protected default ctor/dtor + the above.
- `XYO_PLATFORM_THREAD_LOCAL` — `__declspec(thread)` / `__thread` (trivial
  types only), empty in single thread mode.
- `XYO_PLATFORM_FORMAT_SIZET` — `printf` format for `size_t`.

## Multithreading (`XYO::Platform::Multithreading`)

### TAtomic<T> — header only

`get()` = acquire, `set()` = release, `exchange` / `compareExchange` /
`compareExchangeWeak` / `fetchAdd` / `fetchSub` = acq_rel. Optional last
parameter `std::memory_order`. `operator T()` = `get()`, `operator=` = `set()`.

- Defaults are right for flag + data publication; keep them.
- Pass `std::memory_order_relaxed` only for values that publish nothing
  (statistics counters).
- `set` accepts relaxed/release/seq_cst; `get` relaxed/acquire/seq_cst.
- `a = a + 1` is not atomic → `a.fetchAdd(1)`.
- `compareExchange*` reloads `expected` on failure; `fetchAdd/Sub` return the
  previous value.
- `constexpr` ctors → safe as `static` (constant initialization).
- Single thread build: plain `T`, `isAtomic == false`.

### CriticalSection — mutex

`enter()` / `leave()`. Not copyable/movable.

- **Not recursive on Linux** (Windows CRITICAL_SECTION is): never enter twice
  from the same thread — it works on Windows and deadlocks on Linux.
- No RAII guard in this library; pair enter/leave on every path, or write a
  local guard struct when the protected code can throw.
- Linux ctor/enter/leave throw `std::runtime_error` on pthread failure.
- Single thread build: empty inline methods, `isCriticalSection == false`.

### ConditionVariable — MT only

`wait(cs)`, `waitFor(cs, ms)` (returns false on timeout, negative ms → 0),
`notifyOne()`, `notifyAll()`. `cs` must be entered exactly once by the caller.
Linux uses CLOCK_MONOTONIC. OS errors → `std::runtime_error`.

It has no memory (notify without waiter is lost). Required pattern:

```cpp
// waiter
cs.enter();
while (!isReady) {          // state protected by cs; loop for spurious wake ups
	cv.wait(cs);
};
cs.leave();

// notifier
cs.enter();
isReady = true;             // change state inside cs, then notify
cv.notifyOne();
cs.leave();
```

`waitFor` returning false means "time passed" — still re-check the state.

### Thread — MT only

```cpp
typedef void (*ThreadProcedure)(void *);
bool start(ThreadProcedure procedure, void *this_,
           ThreadProcess before = nullptr, ThreadProcess after = nullptr);
void join();          // idempotent
bool isRunning();     // true from start() until procedure + after finish
static void sleep(int milliSeconds);   // < 0 returns immediately
```

- Function pointer, not std::function: pass state via `this_`
  (`reinterpret_cast<Type *>(this__)` inside). Captureless lambdas convert.
- `start()` joins a previous run first (blocks if still running); objects are
  reusable. `start()` returns false if the OS could not create the thread.
- **Destructor joins** → the procedure must end; stop it cooperatively with a
  `TAtomic<bool>` flag (or state + `notifyAll()` if it waits on a CV).
- **Exceptions escaping the procedure are silently swallowed**: catch/report
  inside.
- `before` / `after` run on the new thread (per-thread init/cleanup); `after`
  runs even if the procedure threw.
- No detach, no kill. Not copyable/movable → arrays or `std::unique_ptr`.
- Single thread build: no class, only the function `Thread::sleep(int)`.

### Processor

`Processor::getCount()` → logical processors, always >= 1.

## Code style (match the repository)

- Tabs, `.clang-format` in the repo; statements and blocks end with `};`.
- camelCase methods, `retV` for return values, `this__` / `this_` for the
  `void *` thread parameter and its cast.
- Headers: include guard `XYO_PLATFORM_<PATH>_HPP`, guarded includes
  `#ifndef XYO_PLATFORM_DEPENDENCY_HPP #include <XYO/Platform/Dependency.hpp>`.
- OS specific code goes in `<Name>-OS-Windows.cpp` / `<Name>-OS-Linux.cpp`
  wrapped in `#ifdef XYO_PLATFORM_OS_*`; public headers keep OS types behind an
  opaque pointer (`class Name_;`). New `.cpp` files must be added to
  `source/XYO/Platform.Amalgam.cpp`; new headers to `source/XYO/Platform.hpp`.
- Source files use CRLF line endings; SPDX header (MIT for `source/`,
  Unlicense for `test/`).
- Tests: `test/test.NN.cpp` + a `"category": "test"` project in
  `fabricare.json`; run `fabricare make` then `fabricare test`. Check Linux
  (`*-OS-Linux.cpp`) through WSL, as described in the fabricare skill.
