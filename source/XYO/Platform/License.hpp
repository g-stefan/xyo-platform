// Platform
// Copyright (c) 2024-2025 Grigore Stefan <g_stefan@yahoo.com>
// MIT License (MIT) <http://opensource.org/licenses/MIT>
// SPDX-FileCopyrightText: 2024-2025 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: MIT

#ifndef XYO_PLATFORM_LICENSE_HPP
#define XYO_PLATFORM_LICENSE_HPP

#ifndef XYO_PLATFORM_DEPENDENCY_HPP
#	include <XYO/Platform/Dependency.hpp>
#endif

namespace XYO::Platform::License {

	XYO_PLATFORM_EXPORT std::string license();
	XYO_PLATFORM_EXPORT std::string shortLicense();
	XYO_PLATFORM_EXPORT std::string licenseMITShort();
	XYO_PLATFORM_EXPORT std::string licenseMITHeader();
	XYO_PLATFORM_EXPORT std::string licenseMITContent();

};

#endif
