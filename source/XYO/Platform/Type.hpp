// Platform
// Copyright (c) 2024-2025 Grigore Stefan <g_stefan@yahoo.com>
// MIT License (MIT) <http://opensource.org/licenses/MIT>
// SPDX-FileCopyrightText: 2024-2025 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: MIT

#ifndef XYO_PLATFORM_TYPE_HPP
#define XYO_PLATFORM_TYPE_HPP

#ifndef XYO_PLATFORM_DEPENDENCY_HPP
#	include <XYO/Platform/Dependency.hpp>
#endif

#define XYO_PLATFORM_DISALLOW_COPY_ASSIGN_MOVE(T) \
private:                                          \
	T(const T &) = delete;                    \
	T(T &&) = delete;                         \
	T &operator=(const T &) = delete;         \
	T &operator=(T &&) = delete

#define XYO_PLATFORM_INTERFACE(T) \
protected:                        \
	T() = default;            \
	~T() = default;           \
	XYO_PLATFORM_DISALLOW_COPY_ASSIGN_MOVE(T)

#endif
