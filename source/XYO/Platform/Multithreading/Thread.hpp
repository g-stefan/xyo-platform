// Platform
// Copyright (c) 2016-2025 Grigore Stefan <g_stefan@yahoo.com>
// MIT License (MIT) <http://opensource.org/licenses/MIT>
// SPDX-FileCopyrightText: 2016-2025 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: MIT

#ifndef XYO_PLATFORM_MULTITHREADING_THREAD_HPP
#define XYO_PLATFORM_MULTITHREADING_THREAD_HPP

#ifndef XYO_PLATFORM_DEPENDENCY_HPP
#	include <XYO/Platform/Dependency.hpp>
#endif
#ifndef XYO_PLATFORM_TYPE_HPP
#	include <XYO/Platform/Type.hpp>
#endif

#ifdef XYO_PLATFORM_SINGLE_THREAD

namespace XYO::Platform::Multithreading::Thread {

	XYO_PLATFORM_EXPORT void sleep(int milliSeconds);

};

#endif

#ifdef XYO_PLATFORM_MULTI_THREAD

namespace XYO::Platform::Multithreading {

	typedef void (*ThreadProcedure)(void *);
	typedef void (*ThreadProcess)(void);
	class Thread_;

	class Thread {
			XYO_PLATFORM_DISALLOW_COPY_ASSIGN_MOVE(Thread);

		protected:
			class Thread_ *thread;

		public:
			XYO_PLATFORM_EXPORT Thread();
			XYO_PLATFORM_EXPORT ~Thread();
			XYO_PLATFORM_EXPORT bool start(ThreadProcedure procedure, void *this_, ThreadProcess before = nullptr, ThreadProcess after = nullptr);
			XYO_PLATFORM_EXPORT void join();
			XYO_PLATFORM_EXPORT bool isRunning();
			XYO_PLATFORM_EXPORT static void sleep(int milliSeconds);
	};

};

#endif

#endif
