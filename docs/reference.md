# API reference

All public symbols, grouped by header. Include `<XYO/Platform.hpp>` to get all
of them. "MT" = available only with `XYO_PLATFORM_MULTI_THREAD`.

## `XYO/Platform/Dependency.hpp`

Included by every other header. Brings in `<cstddef> <cstdint> <cstring>
<cstdio> <cstdlib> <string> <typeinfo> <utility> <stdexcept> <new> <atomic>
<functional> <type_traits>` and `Config.hpp`.

| Macro | Value |
|-------|-------|
| `XYO_PLATFORM_OS_WINDOWS` / `_LINUX` / `_MINGW` / `_EMSCRIPTEN` | target OS ([details](configuration.md#platform-macros)) |
| `XYO_PLATFORM_COMPILER_MSVC` / `_GCC` | compiler family |
| `XYO_PLATFORM_64BIT` / `_32BIT` | pointer size (default 64) |
| `XYO_PLATFORM_MULTI_THREAD` / `_SINGLE_THREAD` | thread support (default multi) |
| `XYO_PLATFORM_NAME` | platform name string literal |
| `XYO_PLATFORM_EXPORT` | dllexport / dllimport / empty ([details](configuration.md#export--linking-macros)) |
| `XYO_PLATFORM_LIBRARY_EXPORT` / `_IMPORT` | compiler keywords used by all XYO libraries |
| `XYO_PLATFORM_THREAD_LOCAL` | `__declspec(thread)` / `__thread` / empty |
| `XYO_PLATFORM_FORMAT_SIZET` | `printf` format for `size_t` |

## `XYO/Platform/Type.hpp`

| Macro | Effect |
|-------|--------|
| `XYO_PLATFORM_DISALLOW_COPY_ASSIGN_MOVE(T)` | deletes copy/move ctor and assignment; leaves `private:` |
| `XYO_PLATFORM_INTERFACE(T)` | protected default ctor/dtor + the above; leaves `private:` |

## `XYO/Platform/Multithreading/TAtomic.hpp`

```cpp
template <typename T>
class TAtomic {
	public:
		static const bool isAtomic;             // false in single thread mode
		static constexpr bool isAlwaysLockFree;

		constexpr TAtomic() noexcept;           // value-initialized
		constexpr TAtomic(const T &value) noexcept;
		TAtomic(const TAtomic &value) noexcept; // copies get()

		void set(const T &value, std::memory_order = std::memory_order_release) noexcept;
		T get(std::memory_order = std::memory_order_acquire) const noexcept;
		T exchange(const T &value, std::memory_order = std::memory_order_acq_rel) noexcept;
		bool compareExchange(T &expected, const T &desired, std::memory_order = std::memory_order_acq_rel) noexcept;
		bool compareExchangeWeak(T &expected, const T &desired, std::memory_order = std::memory_order_acq_rel) noexcept;
		template <typename U> T fetchAdd(U value, std::memory_order = std::memory_order_acq_rel) noexcept;
		template <typename U> T fetchSub(U value, std::memory_order = std::memory_order_acq_rel) noexcept;

		TAtomic &operator=(const TAtomic &value) noexcept; // set(value.get())
		TAtomic &operator=(const T &value) noexcept;       // set(value)
		operator T() const noexcept;                       // get()
};
```

## `XYO/Platform/Multithreading/CriticalSection.hpp`

```cpp
class CriticalSection {                 // not copyable / movable
	public:
		static const bool isCriticalSection; // false in single thread mode
		CriticalSection();                   // Linux: throws std::runtime_error
		~CriticalSection();
		void enter();                        // not recursive on Linux
		void leave();
};
```

## `XYO/Platform/Multithreading/ConditionVariable.hpp` (MT)

```cpp
class ConditionVariable {               // not copyable / movable
	public:
		ConditionVariable();
		~ConditionVariable();
		void wait(CriticalSection &criticalSection);                    // cs entered once
		bool waitFor(CriticalSection &criticalSection, int milliSeconds); // false on timeout
		void notifyOne();
		void notifyAll();
};
```

OS errors throw `std::runtime_error`.

## `XYO/Platform/Multithreading/Thread.hpp`

MT:

```cpp
typedef void (*ThreadProcedure)(void *);
typedef void (*ThreadProcess)(void);

class Thread {                          // not copyable / movable
	public:
		Thread();
		~Thread();                          // joins
		bool start(ThreadProcedure procedure, void *this_,
		           ThreadProcess before = nullptr, ThreadProcess after = nullptr); // joins previous run first
		void join();                        // idempotent
		bool isRunning();
		static void sleep(int milliSeconds); // < 0: returns immediately
};
```

Single thread:

```cpp
namespace XYO::Platform::Multithreading::Thread {
	void sleep(int milliSeconds);
};
```

## `XYO/Platform/Multithreading/Processor.hpp`

```cpp
namespace XYO::Platform::Multithreading::Processor {
	int32_t getCount();                 // logical processors, >= 1
};
```

## `XYO/Platform/Version.hpp`

```cpp
namespace XYO::Platform::Version {
	const char *version();              // "2.0.0"
	const char *build();                // "8"
	const char *versionWithBuild();     // "2.0.0.8"
	const char *datetime();             // "2026-09-24 11:03:33"
};
```

Values come from `version.json` through the generated `Version.rh`.

## `XYO/Platform/License.hpp`

```cpp
namespace XYO::Platform::License {
	std::string license();              // header + copyright + MIT text
	std::string shortLicense();         // copyright + one line MIT reference
	std::string licenseMITShort();      // "MIT License (MIT) <http://...>"
	std::string licenseMITHeader();     // "MIT License" title
	std::string licenseMITContent();    // MIT permission and warranty text
};
```

## `XYO/Platform/Copyright.hpp`

```cpp
namespace XYO::Platform::Copyright {
	const char *copyright();
	const char *publisher();
	const char *company();
	const char *contact();
};
```
