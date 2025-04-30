// Platform
// Copyright (c) 2016-2025 Grigore Stefan <g_stefan@yahoo.com>
// MIT License (MIT) <http://opensource.org/licenses/MIT>
// SPDX-FileCopyrightText: 2016-2025 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: MIT

#include <XYO/Platform/Dependency.hpp>
#include <XYO/Platform/Multithreading/Thread.hpp>

#ifdef XYO_PLATFORM_OS_WINDOWS

#	include <stdio.h>
#	define WIN32_LEAN_AND_MEAN
#	include <windows.h>

#	ifdef XYO_PLATFORM_SINGLE_THREAD
namespace XYO::Platform::Multithreading::Thread {

	void sleep(int milliSeconds) {
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
	};

	static DWORD WINAPI threadProcedure_(Thread_ *thread) {
		if (thread->before != nullptr) {
			(*thread->before)();
		};
		(*thread->procedure)(thread->this_);
		if (thread->after != nullptr) {
			(*thread->after)();
		};
		thread->id = 0;
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
	};

	Thread::~Thread() {
		join();
		delete thread;
	};

	void Thread::join() {
		if (thread->id) {
			WaitForSingleObject(thread->thread, INFINITE);
			CloseHandle(thread->thread);
			thread->id = 0;
		};
	};

	bool Thread::start(ThreadProcedure procedure, void *this_, ThreadProcess before, ThreadProcess after) {
		join();

		thread->procedure = procedure;
		thread->this_ = this_;
		thread->before = before;
		thread->after = after;

		thread->thread = CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)threadProcedure_, thread, 0, &thread->id);
		if (thread->thread != nullptr) {
			return true;
		};
		return false;
	};

	bool Thread::isRunning() {
		if (thread->id) {
			return true;
		};
		return false;
	};

	void Thread::sleep(int milliSeconds) {
		Sleep(milliSeconds);
	};

};
#	endif

#endif
