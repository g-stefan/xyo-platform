// Platform
// Copyright (c) 2016-2025 Grigore Stefan <g_stefan@yahoo.com>
// MIT License (MIT) <http://opensource.org/licenses/MIT>
// SPDX-FileCopyrightText: 2016-2025 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: MIT

#include <XYO/Platform/Dependency.hpp>
#include <XYO/Platform/Multithreading/CriticalSection.hpp>

#ifdef XYO_PLATFORM_OS_LINUX

#	include <pthread.h>
#	include <unistd.h>

#	ifdef XYO_PLATFORM_MULTI_THREAD

namespace XYO::Platform::Multithreading {

	class CriticalSection_ {
		public:
			pthread_mutex_t section;
	};

	CriticalSection::CriticalSection() {
		criticalSection = new CriticalSection_();
		if (pthread_mutex_init(&criticalSection->section, nullptr)) {
			throw std::runtime_error("pthread_mutex_init");
		};
	};

	CriticalSection::~CriticalSection() noexcept(false) {
		if (pthread_mutex_destroy(&criticalSection->section)) {
			throw std::runtime_error("pthread_mutex_destroy");
		};
		delete criticalSection;
	};

	void CriticalSection::enter() {
		if (pthread_mutex_lock(&criticalSection->section)) {
			throw std::runtime_error("pthread_mutex_lock");
		};
	};

	void CriticalSection::leave() {
		if (pthread_mutex_unlock(&criticalSection->section)) {
			throw std::runtime_error("pthread_mutex_unlock");
		};
	};

};

#	endif

#endif
