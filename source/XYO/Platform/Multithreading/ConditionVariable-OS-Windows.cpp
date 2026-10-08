// Platform
// Copyright (c) 2016-2026 Grigore Stefan <g_stefan@yahoo.com>
// MIT License (MIT) <http://opensource.org/licenses/MIT>
// SPDX-FileCopyrightText: 2016-2026 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: MIT

#include <XYO/Platform/Dependency.hpp>
#include <XYO/Platform/Multithreading/ConditionVariable.hpp>

#ifdef XYO_PLATFORM_OS_WINDOWS

#	define WIN32_LEAN_AND_MEAN
#	include <windows.h>

#	ifdef XYO_PLATFORM_MULTI_THREAD

namespace XYO::Platform::Multithreading {

	class ConditionVariable_ {
		public:
			CONDITION_VARIABLE conditionVariable;
	};

	ConditionVariable::ConditionVariable() {
		conditionVariable = new ConditionVariable_();
		InitializeConditionVariable(&conditionVariable->conditionVariable);
	};

	// CONDITION_VARIABLE has no destroy function
	ConditionVariable::~ConditionVariable() {
		delete conditionVariable;
	};

	void ConditionVariable::wait(CriticalSection &criticalSection) {
		if (!SleepConditionVariableCS(&conditionVariable->conditionVariable,
		                              reinterpret_cast<CRITICAL_SECTION *>(criticalSection.getNative_()),
		                              INFINITE)) {
			throw std::runtime_error("SleepConditionVariableCS");
		};
	};

	bool ConditionVariable::waitFor(CriticalSection &criticalSection, int milliSeconds) {
		if (milliSeconds < 0) {
			milliSeconds = 0;
		};
		if (!SleepConditionVariableCS(&conditionVariable->conditionVariable,
		                              reinterpret_cast<CRITICAL_SECTION *>(criticalSection.getNative_()),
		                              (DWORD)milliSeconds)) {
			if (GetLastError() == ERROR_TIMEOUT) {
				return false;
			};
			throw std::runtime_error("SleepConditionVariableCS");
		};
		return true;
	};

	void ConditionVariable::notifyOne() {
		WakeConditionVariable(&conditionVariable->conditionVariable);
	};

	void ConditionVariable::notifyAll() {
		WakeAllConditionVariable(&conditionVariable->conditionVariable);
	};

};

#	endif

#endif
