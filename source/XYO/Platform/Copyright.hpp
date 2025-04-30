// Platform
// Copyright (c) 2024-2025 Grigore Stefan <g_stefan@yahoo.com>
// MIT License (MIT) <http://opensource.org/licenses/MIT>
// SPDX-FileCopyrightText: 2024-2025 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: MIT

#ifndef XYO_PLATFORM_COPYRIGHT_HPP
#define XYO_PLATFORM_COPYRIGHT_HPP

#ifndef XYO_PLATFORM_DEPENDENCY_HPP
#	include <XYO/Platform/Dependency.hpp>
#endif

namespace XYO::Platform::Copyright {
	XYO_PLATFORM_EXPORT const char *copyright();
	XYO_PLATFORM_EXPORT const char *publisher();
	XYO_PLATFORM_EXPORT const char *company();
	XYO_PLATFORM_EXPORT const char *contact();
};

#endif
