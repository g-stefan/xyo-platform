// Platform
// Copyright (c) 2016-2025 Grigore Stefan <g_stefan@yahoo.com>
// MIT License (MIT) <http://opensource.org/licenses/MIT>
// SPDX-FileCopyrightText: 2016-2025 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: MIT

#ifndef XYO_PLATFORM_MULTITHREADING_CRITICALSECTION_HPP
#define XYO_PLATFORM_MULTITHREADING_CRITICALSECTION_HPP

#ifndef XYO_PLATFORM_DEPENDENCY_HPP
#	include <XYO/Platform/Dependency.hpp>
#endif
#ifndef XYO_PLATFORM_TYPE_HPP
#	include <XYO/Platform/Type.hpp>
#endif

#ifdef XYO_PLATFORM_MULTI_THREAD
namespace XYO::Platform::Multithreading {

	class CriticalSection_;

	class CriticalSection {
			XYO_PLATFORM_DISALLOW_COPY_ASSIGN_MOVE(CriticalSection);

		protected:
			CriticalSection_ *criticalSection;

		public:
			static const bool isCriticalSection = true;

			XYO_PLATFORM_EXPORT CriticalSection();
#	ifdef XYO_PLATFORM_OS_LINUX
			XYO_PLATFORM_EXPORT ~CriticalSection() noexcept(false);
#	else
			XYO_PLATFORM_EXPORT ~CriticalSection();
#	endif

			XYO_PLATFORM_EXPORT void enter();
			XYO_PLATFORM_EXPORT void leave();
	};

};
#endif

#ifdef XYO_PLATFORM_SINGLE_THREAD
namespace XYO::Platform {

	class CriticalSection {
			XYO_PLATFORM_DISALLOW_COPY_ASSIGN_MOVE(CriticalSection);

		public:
			static const bool isCriticalSection = false;

			CriticalSection() = default;

			inline void enter(){};

			inline void leave(){};
	};

};
#endif

#endif
