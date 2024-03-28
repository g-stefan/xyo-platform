// Platform
// Copyright (c) 2024 Grigore Stefan <g_stefan@yahoo.com>
// MIT License (MIT) <http://opensource.org/licenses/MIT>
// SPDX-FileCopyrightText: 2024 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: MIT

#ifndef XYO_PLATFORM_DEPENDENCY_HPP
#define XYO_PLATFORM_DEPENDENCY_HPP

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <typeinfo>
#include <utility>
#include <stdexcept>
#include <new>
#include <atomic>
#include <functional>
#include <type_traits>

#ifndef XYO_PLATFORM_CONFIG_HPP
#	include <XYO/Platform/Config.hpp>
#endif

// -- Export

#ifdef XYO_PLATFORM_COMPILE_DYNAMIC_LIBRARY
#	ifndef XYO_PLATFORM_LIBRARY_EXPORT
#		ifdef XYO_PLATFORM_COMPILER_MSVC
#			define XYO_PLATFORM_LIBRARY_EXPORT __declspec(dllexport)
#		endif
#		ifdef XYO_PLATFORM_COMPILER_GCC
#			define XYO_PLATFORM_LIBRARY_EXPORT
#		endif
#	endif
#	ifndef XYO_PLATFORM_LIBRARY_IMPORT
#		ifdef XYO_PLATFORM_COMPILER_MSVC
#			define XYO_PLATFORM_LIBRARY_IMPORT __declspec(dllimport)
#		endif
#		ifdef XYO_PLATFORM_COMPILER_GCC
#			define XYO_PLATFORM_LIBRARY_IMPORT
#		endif
#	endif
#else
#	ifndef XYO_PLATFORM_LIBRARY_EXPORT
#		define XYO_PLATFORM_LIBRARY_EXPORT
#	endif
#	ifndef XYO_PLATFORM_LIBRARY_IMPORT
#		define XYO_PLATFORM_LIBRARY_IMPORT
#	endif
#endif

#ifdef XYO_PLATFORM_INTERNAL
#	define XYO_PLATFORM_EXPORT XYO_PLATFORM_LIBRARY_EXPORT
#else
#	define XYO_PLATFORM_EXPORT XYO_PLATFORM_LIBRARY_IMPORT
#endif

// -- Defaults

#ifndef XYO_PLATFORM_MULTI_THREAD
#	ifndef XYO_PLATFORM_SINGLE_THREAD
#		define XYO_PLATFORM_MULTI_THREAD
#	endif
#endif

#ifndef XYO_PLATFORM_64BIT
#	ifndef XYO_PLATFORM_32BIT
#		define XYO_PLATFORM_64BIT
#	endif
#endif

#ifdef XYO_PLATFORM_OS_MINGW
#	ifndef XYO_PLATFORM_OS_WINDOWS
#		define XYO_PLATFORM_OS_WINDOWS
#	endif
#endif

// -- Checking

#ifndef XYO_PLATFORM_COMPILER_MSVC
#	ifndef XYO_PLATFORM_COMPILER_GCC
#		error "XYO_PLATFORM_COMPILER_MSVC or XYO_PLATFORM_COMPILER_GCC must be defined"
#	endif
#endif

#ifndef XYO_PLATFORM_OS_WINDOWS
#	ifndef XYO_PLATFORM_OS_LINUX
#		ifndef XYO_PLATFORM_OS_MINGW
#			error "XYO_PLATFORM_OS_WINDOWS or XYO_PLATFORM_OS_LINUX or XYO_PLATFORM_OS_MINGW must be defined"
#		endif
#	endif
#endif

#ifndef XYO_PLATFORM_MULTI_THREAD
#	ifndef XYO_PLATFORM_SINGLE_THREAD
#		error "XYO_PLATFORM_MULTI_THREAD or XYO_PLATFORM_SINGLE_THREAD must be defined"
#	endif
#endif

#ifndef XYO_PLATFORM_64BIT
#	ifndef XYO_PLATFORM_32BIT
#		error "XYO_PLATFORM_64BIT or XYO_PLATFORM_32BIT must be defined"
#	endif
#endif

//

#ifdef XYO_PLATFORM_MULTI_THREAD
#	ifdef XYO_PLATFORM_COMPILER_MSVC
#		define XYO_PLATFORM_THREAD_LOCAL __declspec(thread)
#	endif
#	ifdef XYO_PLATFORM_COMPILER_GCC
#		define XYO_PLATFORM_THREAD_LOCAL __thread
#	endif
#else
#	define XYO_PLATFORM_THREAD_LOCAL
#endif

//

#ifdef XYO_PLATFORM_64BIT
#	ifdef XYO_PLATFORM_COMPILER_GCC
#		define XYO_PLATFORM_FORMAT_SIZET "%lu"
#	else
#		define XYO_PLATFORM_FORMAT_SIZET "%llu"
#	endif
#else
#	define XYO_PLATFORM_FORMAT_SIZET "%lu"
#endif

//

#ifdef XYO_PLATFORM_OS_WINDOWS
#	ifdef XYO_PLATFORM_MEMORY_LEAK_DETECTOR_VLD
#		ifndef WIN32_LEAN_AND_MEAN
#			define WIN32_LEAN_AND_MEAN
#		endif
#		include <windows.h>
#		include "vld.h"
#	endif
#endif

#endif
