// Platform
// Copyright (c) 2016-2026 Grigore Stefan <g_stefan@yahoo.com>
// MIT License (MIT) <http://opensource.org/licenses/MIT>
// SPDX-FileCopyrightText: 2016-2026 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: MIT

#include <XYO/Platform/Dependency.hpp>
#include <XYO/Platform/Multithreading/TAtomic.hpp>
#include <XYO/Platform/Multithreading/Thread.hpp>

#ifdef XYO_PLATFORM_OS_WINDOWS

#	include <stdio.h>
#	define WIN32_LEAN_AND_MEAN
#	include <windows.h>

#	ifdef XYO_PLATFORM_SINGLE_THREAD
namespace XYO::Platform::Multithreading::Thread {

	void sleep(int milliSeconds) {
		if (milliSeconds < 0) {
			return;
		};
		Sleep(milliSeconds);
	};

};
#	endif

#	ifdef XYO_PLATFORM_MULTI_THREAD
namespace XYO::Platform::Multithreading {

	struct Thread_ {
			ThreadProcedure procedure;
			void *this_;
			ThreadProcess before;
			ThreadProcess after;
			// ---
			DWORD id;
			HANDLE thread;
			// ---
			TAtomic<bool> isRunning;
			bool joinable;
	};

	static DWORD WINAPI threadProcedure_(void *thread_) {
		Thread_ *thread = static_cast<Thread_ *>(thread_);
		if (thread->before != nullptr) {
			(*thread->before)();
		};
		try {
			(*thread->procedure)(thread->this_);
		} catch (...) {
		}
		if (thread->after != nullptr) {
			(*thread->after)();
		};
		thread->isRunning.set(false);
		return 0L;
	};

	Thread::Thread() {
		thread = new Thread_();
		thread->procedure = nullptr;
		thread->this_ = nullptr;
		thread->before = nullptr;
		thread->after = nullptr;
		thread->id = 0;
		thread->thread = nullptr;
		thread->isRunning.set(false);
		thread->joinable = false;
	};

	Thread::~Thread() {
		join();
		delete thread;
	};

	void Thread::join() {
		if (thread->joinable) {
			WaitForSingleObject(thread->thread, INFINITE);
			CloseHandle(thread->thread);
			thread->thread = nullptr;
			thread->id = 0;
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
		thread->thread = CreateThread(nullptr, 0, threadProcedure_, thread, 0, &thread->id);
		if (thread->thread != nullptr) {
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
		Sleep(milliSeconds);
	};

};
#	endif

#endif
