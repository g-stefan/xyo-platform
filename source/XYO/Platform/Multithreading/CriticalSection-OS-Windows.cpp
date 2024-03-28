// Platform
// Copyright (c) 2016-2024 Grigore Stefan <g_stefan@yahoo.com>
// MIT License (MIT) <http://opensource.org/licenses/MIT>
// SPDX-FileCopyrightText: 2016-2024 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: MIT

#include <XYO/Platform/Dependency.hpp>
#include <XYO/Platform/Multithreading/CriticalSection.hpp>

#ifdef XYO_PLATFORM_OS_WINDOWS

#	define WIN32_LEAN_AND_MEAN
#	include <windows.h>

#	ifdef XYO_PLATFORM_MULTI_THREAD

namespace XYO::Platform::Multithreading {

	class CriticalSection_ {
		public:
			CRITICAL_SECTION section;
	};

	CriticalSection::CriticalSection() {
		criticalSection = new CriticalSection_();
		InitializeCriticalSection(&criticalSection->section);
	};

	CriticalSection::~CriticalSection() {
		DeleteCriticalSection(&criticalSection->section);
		delete criticalSection;
	};

	void CriticalSection::enter() {
		EnterCriticalSection(&criticalSection->section);
	};

	void CriticalSection::leave() {
		LeaveCriticalSection(&criticalSection->section);
	};

};

#	endif

#endif
