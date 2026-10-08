// Platform
// Copyright (c) 2016-2026 Grigore Stefan <g_stefan@yahoo.com>
// MIT License (MIT) <http://opensource.org/licenses/MIT>
// SPDX-FileCopyrightText: 2016-2026 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: MIT

#ifndef XYO_PLATFORM_MULTITHREADING_CONDITIONVARIABLE_HPP
#define XYO_PLATFORM_MULTITHREADING_CONDITIONVARIABLE_HPP

#ifndef XYO_PLATFORM_DEPENDENCY_HPP
#	include <XYO/Platform/Dependency.hpp>
#endif
#ifndef XYO_PLATFORM_MULTITHREADING_CRITICALSECTION_HPP
#	include <XYO/Platform/Multithreading/CriticalSection.hpp>
#endif

#ifdef XYO_PLATFORM_MULTI_THREAD
namespace XYO::Platform::Multithreading {

	class ConditionVariable_;

	//
	// Condition variable, used with a CriticalSection:
	// a thread waits (sleeps, no CPU) until another thread notifies it.
	// Windows: CONDITION_VARIABLE, Linux: pthread_cond_t.
	//
	// It has no state: a notify when no thread waits is lost. Keep the
	// state (the condition) in variables protected by the CriticalSection,
	// change it and notify with the CriticalSection entered, and wait in
	// a loop checking the state (a wait can also return without notify):
	//
	//   // waiting thread
	//   criticalSection.enter();
	//   while (!isReady) {
	//       conditionVariable.wait(criticalSection);
	//   };
	//   criticalSection.leave();
	//
	//   // notifying thread
	//   criticalSection.enter();
	//   isReady = true;
	//   conditionVariable.notifyOne();
	//   criticalSection.leave();
	//
	// - wait() / waitFor() must be called with criticalSection entered
	//   exactly once by the calling thread, it is left while waiting and
	//   entered again before return
	// - waitFor() returns false on timeout
	// - notifyOne() wakes one waiting thread, notifyAll() all of them
	//
	class ConditionVariable {
			XYO_PLATFORM_DISALLOW_COPY_ASSIGN_MOVE(ConditionVariable);

		protected:
			ConditionVariable_ *conditionVariable;

		public:
			XYO_PLATFORM_EXPORT ConditionVariable();
			XYO_PLATFORM_EXPORT ~ConditionVariable();

			XYO_PLATFORM_EXPORT void wait(CriticalSection &criticalSection);
			XYO_PLATFORM_EXPORT bool waitFor(CriticalSection &criticalSection, int milliSeconds);
			XYO_PLATFORM_EXPORT void notifyOne();
			XYO_PLATFORM_EXPORT void notifyAll();
	};

};
#endif

#endif
