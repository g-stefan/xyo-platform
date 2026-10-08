// Platform
// Copyright (c) 2016-2026 Grigore Stefan <g_stefan@yahoo.com>
// MIT License (MIT) <http://opensource.org/licenses/MIT>
// SPDX-FileCopyrightText: 2016-2026 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: MIT

#include <XYO/Platform/Dependency.hpp>
#include <XYO/Platform/Multithreading/ConditionVariable.hpp>

#ifdef XYO_PLATFORM_OS_LINUX

#	include <pthread.h>
#	include <time.h>
#	include <errno.h>

#	ifdef XYO_PLATFORM_MULTI_THREAD

namespace XYO::Platform::Multithreading {

	class ConditionVariable_ {
		public:
			pthread_cond_t conditionVariable;
	};

	// waitFor() uses CLOCK_MONOTONIC, not affected by system time change
	ConditionVariable::ConditionVariable() {
		pthread_condattr_t attributes;
		conditionVariable = new ConditionVariable_();
		if (pthread_condattr_init(&attributes)) {
			delete conditionVariable;
			conditionVariable = nullptr;
			throw std::runtime_error("pthread_condattr_init");
		};
		if (pthread_condattr_setclock(&attributes, CLOCK_MONOTONIC) ||
		    pthread_cond_init(&conditionVariable->conditionVariable, &attributes)) {
			pthread_condattr_destroy(&attributes);
			delete conditionVariable;
			conditionVariable = nullptr;
			throw std::runtime_error("pthread_cond_init");
		};
		pthread_condattr_destroy(&attributes);
	};

	ConditionVariable::~ConditionVariable() {
		if (conditionVariable) {
			pthread_cond_destroy(&conditionVariable->conditionVariable);
		};
		delete conditionVariable;
	};

	void ConditionVariable::wait(CriticalSection &criticalSection) {
		if (pthread_cond_wait(&conditionVariable->conditionVariable,
		                      reinterpret_cast<pthread_mutex_t *>(criticalSection.getNative_()))) {
			throw std::runtime_error("pthread_cond_wait");
		};
	};

	bool ConditionVariable::waitFor(CriticalSection &criticalSection, int milliSeconds) {
		struct timespec time;
		int retV;

		if (milliSeconds < 0) {
			milliSeconds = 0;
		};
		clock_gettime(CLOCK_MONOTONIC, &time);
		time.tv_sec += milliSeconds / 1000;
		time.tv_nsec += (long)(milliSeconds % 1000) * 1000000L;
		if (time.tv_nsec >= 1000000000L) {
			time.tv_sec += 1;
			time.tv_nsec -= 1000000000L;
		};

		retV = pthread_cond_timedwait(&conditionVariable->conditionVariable,
		                              reinterpret_cast<pthread_mutex_t *>(criticalSection.getNative_()),
		                              &time);
		if (retV == ETIMEDOUT) {
			return false;
		};
		if (retV) {
			throw std::runtime_error("pthread_cond_timedwait");
		};
		return true;
	};

	void ConditionVariable::notifyOne() {
		pthread_cond_signal(&conditionVariable->conditionVariable);
	};

	void ConditionVariable::notifyAll() {
		pthread_cond_broadcast(&conditionVariable->conditionVariable);
	};

};

#	endif

#endif
