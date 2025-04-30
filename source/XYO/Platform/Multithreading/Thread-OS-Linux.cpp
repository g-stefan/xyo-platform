// Platform
// Copyright (c) 2016-2025 Grigore Stefan <g_stefan@yahoo.com>
// MIT License (MIT) <http://opensource.org/licenses/MIT>
// SPDX-FileCopyrightText: 2016-2025 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: MIT

#include <XYO/Platform/Dependency.hpp>
#include <XYO/Platform/Multithreading/Thread.hpp>

#ifdef XYO_PLATFORM_OS_LINUX

#	include <pthread.h>
#	include <unistd.h>
#	include <time.h>

#	ifdef XYO_PLATFORM_SINGLE_THREAD
namespace XYO::Platform::Multithreading::Thread {

	void sleep(int milliSeconds) {
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
			bool isRunning;
			ThreadProcess before;
			ThreadProcess after;
			// ---
			pthread_t thread;
	};

	static void *threadProcedure_(Thread_ *thread) {
		if (thread->before != nullptr) {
			(*thread->before)();
		};
		(*thread->procedure)(thread->this_);
		if (thread->after != nullptr) {
			(*thread->after)();
		};
		thread->isRunning = false;
		return nullptr;
	};

	Thread::Thread() {
		thread = new Thread_();
		thread->procedure = nullptr;
		thread->this_ = nullptr;
		thread->before = nullptr;
		thread->after = nullptr;
		thread->isRunning = false;
	};

	Thread::~Thread() {
		join();
		delete thread;
	};

	void Thread::join() {
		if (thread->isRunning) {
			if (pthread_join(thread->thread, nullptr)) {
				throw std::runtime_error("pthread_join");
			};
			thread->isRunning = false;
		};
	};

	bool Thread::start(ThreadProcedure procedure, void *this_, ThreadProcess before, ThreadProcess after) {
		join();

		thread->procedure = procedure;
		thread->this_ = this_;
		thread->before = before;
		thread->after = after;

		if (pthread_create(&thread->thread, nullptr, (void *(*)(void *))threadProcedure_, thread) == 0) {
			thread->isRunning = true;
			return true;
		};
		return false;
	};

	bool Thread::isRunning() {
		return thread->isRunning;
	};

	void Thread::sleep(int milliSeconds) {
		struct timespec _sleep;
		_sleep.tv_sec = milliSeconds / 1000;
		_sleep.tv_nsec = (milliSeconds % 1000) * 1000000;
		while (nanosleep(&_sleep, &_sleep)) {
		};
	};

};

#	endif

#endif
