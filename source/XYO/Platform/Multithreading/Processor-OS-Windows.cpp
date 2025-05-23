// Platform
// Copyright (c) 2016-2025 Grigore Stefan <g_stefan@yahoo.com>
// MIT License (MIT) <http://opensource.org/licenses/MIT>
// SPDX-FileCopyrightText: 2016-2025 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: MIT

#include <XYO/Platform/Dependency.hpp>
#include <XYO/Platform/Multithreading/Processor.hpp>

#ifdef XYO_PLATFORM_OS_WINDOWS

#	define WIN32_LEAN_AND_MEAN
#	include <windows.h>
#	include <stdlib.h>
#	include <stdio.h>
#	include <string.h>

namespace XYO::Platform::Multithreading::Processor {

	int32_t getCount() {
		return GetActiveProcessorCount(ALL_PROCESSOR_GROUPS);
	};

};

#endif
