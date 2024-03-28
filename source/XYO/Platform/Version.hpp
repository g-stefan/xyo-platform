// Platform
// Copyright (c) 2024 Grigore Stefan <g_stefan@yahoo.com>
// MIT License (MIT) <http://opensource.org/licenses/MIT>
// SPDX-FileCopyrightText: 2024 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: MIT

#ifndef XYO_PLATFORM_VERSION_HPP
#define XYO_PLATFORM_VERSION_HPP

#ifndef XYO_PLATFORM_DEPENDENCY_HPP
#	include <XYO/Platform/Dependency.hpp>
#endif

namespace XYO::Platform::Version {

	XYO_PLATFORM_EXPORT const char *version();
	XYO_PLATFORM_EXPORT const char *build();
	XYO_PLATFORM_EXPORT const char *versionWithBuild();
	XYO_PLATFORM_EXPORT const char *datetime();

};

#endif
