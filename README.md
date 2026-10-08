# Platform

C++ library
- Platform abstraction

Base layer of the XYO C++ libraries: compile-time platform configuration
(`XYO_PLATFORM_*` macros), DLL export macros and minimal multithreading
primitives (`TAtomic`, `CriticalSection`, `ConditionVariable`, `Thread`,
`Processor`).

## Documentation

- [Overview](docs/README.md) - purpose and design
- [Getting started](docs/getting-started.md) - build, depend on it, first program
- [Configuration](docs/configuration.md) - `XYO_PLATFORM_*` macros
- [Multithreading](docs/multithreading.md) - usage and pitfalls
- [API reference](docs/reference.md)

A Claude Code skill for this library is in
[.claude/skills/xyo-platform](.claude/skills/xyo-platform/SKILL.md).

## License

Copyright (c) 2024-2026 Grigore Stefan
Licensed under the [MIT](LICENSE) license.
