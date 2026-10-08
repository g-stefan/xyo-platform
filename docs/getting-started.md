# Getting started

## 1. Build and install

The library is built with [fabricare](https://github.com/g-stefan/fabricare),
the build tool used by all XYO C++ projects. From the repository root:

```bash
fabricare make       # configure + build into output/
fabricare test       # build and run test/test.*.cpp (run make first)
fabricare install    # copy output/{bin,include,lib} to ~/.fabricare/<platform>
fabricare clean      # remove output/ and temp/
```

`make` first runs `fabricare/make.config.js`: it compiles and runs
`source/XYO/Platform.Config.cpp`, which writes
`source/XYO/Platform/Config.hpp` for the current platform (see
[Configuration](configuration.md)). Then two libraries are produced:

| Project                 | Kind                          | Use it when                                 |
|-------------------------|-------------------------------|---------------------------------------------|
| `xyo-platform`          | DLL / shared library (`dll-or-lib`) | default, shared between several libraries |
| `xyo-platform.static`   | static library, static CRT    | self-contained executables                  |

`install` publishes them to the SDK repository so other projects can depend
on them.

## 2. Depend on it from another fabricare project

In the consumer's `fabricare.json`:

```json
{
	"name": "my-library",
	"make": "dll-or-lib",
	"sourcePath": "XYO/MyLibrary",
	"dependency": [
		"xyo-platform"
	]
}
```

For the static variant use `"xyo-platform.static"`. It exports the define
`XYO_PLATFORM_LIBRARY` to the consumer automatically (`dependencyDefines`), which
turns `XYO_PLATFORM_EXPORT` into nothing, so no `__declspec(dllimport)` is
generated.

## 3. Include

Always include the umbrella header:

```cpp
#include <XYO/Platform.hpp>
```

It pulls in the configuration, the standard headers the library relies on
(`<cstdint>`, `<string>`, `<atomic>`, `<functional>`, `<stdexcept>`, ...) and all
public classes. Individual headers (`<XYO/Platform/Multithreading/Thread.hpp>`,
...) also work on their own.

Libraries built on top of `xyo-platform` usually re-export the names they
need in their own `Dependency.hpp`:

```cpp
#include <XYO/Platform.hpp>

namespace XYO::MyLibrary {
	using XYO::Platform::Multithreading::CriticalSection;
	using XYO::Platform::Multithreading::TAtomic;
};
```

## 4. First program

```cpp
#include <XYO/Platform.hpp>

using namespace XYO::Platform;
using namespace XYO::Platform::Multithreading;

struct Counter {
	TAtomic<int> value;
};

void work(void *this__) {
	Counter *this_ = reinterpret_cast<Counter *>(this__);
	for (int k = 0; k < 1000; ++k) {
		this_->value.fetchAdd(1);
	};
};

int main(int, char *[]) {
	printf("xyo-platform %s on %s, %d processors\n",
	       Version::versionWithBuild(),
	       XYO_PLATFORM_NAME,
	       Processor::getCount());

	Counter counter;
	Thread threads[4];
	for (auto &thread : threads) {
		thread.start(work, &counter);
	};
	for (auto &thread : threads) {
		thread.join();
	};

	printf("count = %d\n", counter.value.get()); // 4000
	return 0;
};
```

As a fabricare test project:

```json
{
	"name": "test.03",
	"make": "exe",
	"category": "test",
	"dependency": [
		"xyo-platform"
	]
}
```

with the source in `test/test.03.cpp`.

## 5. Building without fabricare

The whole library is also available as one translation unit,
`source/XYO/Platform.Amalgam.cpp`, used to embed it into other builds
(for example fabricare's own bootstrap build). To use it:

1. Put `source/` on the include path.
2. Provide `source/XYO/Platform/Config.hpp`: either generate it by compiling
   and running `Platform.Config.cpp` with the platform macros defined
   (run it from the repository root, it needs an existing `temp/` folder), or
   write it by hand from `Config.Template.hpp` by uncommenting the right lines.
3. Compile `Platform.Amalgam.cpp` together with your sources and define
   `XYO_PLATFORM_LIBRARY` everywhere (plain static linking, no import/export).
4. Link `pthread` on Linux.

Example on Linux:

```bash
mkdir -p temp
g++ -std=c++17 -Isource \
    -DXYO_PLATFORM_OS_LINUX -DXYO_PLATFORM_COMPILER_GCC \
    -DXYO_PLATFORM_64BIT -DXYO_PLATFORM_MULTI_THREAD \
    -DXYO_PLATFORM_NAME=ubuntu \
    source/XYO/Platform.Config.cpp -o temp/config && ./temp/config
g++ -std=c++17 -Isource -DXYO_PLATFORM_LIBRARY \
    source/XYO/Platform.Amalgam.cpp main.cpp -o main -pthread
```

The `Version.rh` file used by `Version.cpp` is in the repository; `fabricare
version` updates it from `version.json`.
