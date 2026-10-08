// Platform
// Copyright (c) 2016-2026 Grigore Stefan <g_stefan@yahoo.com>
// MIT License (MIT) <http://opensource.org/licenses/MIT>
// SPDX-FileCopyrightText: 2016-2026 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: MIT

#include <XYO/Platform/Dependency.hpp>
#include <XYO/Platform/Multithreading/TAtomic.hpp>
#include <XYO/Platform/Multithreading/Thread.hpp>

#ifdef XYO_PLATFORM_OS_LINUX

#	include <pthread.h>
#	include <unistd.h>
#	include <time.h>
#	ifdef __GLIBCXX__
#		include <cxxabi.h>
#	endif

#	ifdef XYO_PLATFORM_SINGLE_THREAD
namespace XYO::Platform::Multithreading::Thread {

	void sleep(int milliSeconds) {
		if (milliSeconds < 0) {
			return;
		};
		struct timespec _sleep;
		_sleep.tv_sec = milliSeconds / 1000;
		_sleep.tv_nsec = (milliSeconds % 1000) * 1000000;
		while (nanosleep(&_sleep, &_sleep)) {
		};
	};

};
#	endif

#	ifdef XYO_PLATFORM_MULTI_THREAD
namespace XYO::Platform::Multithreading {

	struct Thread_ {
			ThreadProcedure procedure;
			void *this_;
			TAtomic<bool> isRunning;
			ThreadProcess before;
			ThreadProcess after;
			// ---
			pthread_t thread;
			bool joinable;
	};

	static void *threadProcedure_(void *thread_) {
		Thread_ *thread = static_cast<Thread_ *>(thread_);
		if (thread->before != nullptr) {
			(*thread->before)();
		};
#		ifdef __GLIBCXX__
		try {
			(*thread->procedure)(thread->this_);
		} catch (abi::__forced_unwind &) {
			if (thread->after != nullptr) {
				(*thread->after)();
			};
			thread->isRunning.set(false);
			throw;
		} catch (...) {
		}
#		else
		try {
			(*thread->procedure)(thread->this_);
		} catch (...) {
		}
#		endif
		if (thread->after != nullptr) {
			(*thread->after)();
		};
		thread->isRunning.set(false);
		return nullptr;
	};

	Thread::Thread() {
		thread = new Thread_();
		thread->procedure = nullptr;
		thread->this_ = nullptr;
		thread->before = nullptr;
		thread->after = nullptr;
		thread->isRunning.set(false);
		thread->joinable = false;
	};

	Thread::~Thread() {
		join();
		delete thread;
	};

	void Thread::join() {
		if (thread->joinable) {
			pthread_join(thread->thread, nullptr);
			thread->joinable = false;
		};
		thread->isRunning.set(false);
	};

	bool Thread::start(ThreadProcedure procedure, void *this_, ThreadProcess before, ThreadProcess after) {
		join();

		thread->procedure = procedure;
		thread->this_ = this_;
		thread->before = before;
		thread->after = after;

		thread->isRunning.set(true);
		if (pthread_create(&thread->thread, nullptr, threadProcedure_, thread) == 0) {
			thread->joinable = true;
			return true;
		};
		thread->isRunning.set(false);
		return false;
	};

	bool Thread::isRunning() {
		return thread->isRunning.get();
	};

	void Thread::sleep(int milliSeconds) {
		if (milliSeconds < 0) {
			return;
		};
		struct timespec _sleep;
		_sleep.tv_sec = milliSeconds / 1000;
		_sleep.tv_nsec = (milliSeconds % 1000) * 1000000;
		while (nanosleep(&_sleep, &_sleep)) {
		};
	};

};

#	endif

#endif
