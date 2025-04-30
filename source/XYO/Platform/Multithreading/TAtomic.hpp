// Platform
// Copyright (c) 2016-2025 Grigore Stefan <g_stefan@yahoo.com>
// MIT License (MIT) <http://opensource.org/licenses/MIT>
// SPDX-FileCopyrightText: 2016-2025 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: MIT

#ifndef XYO_PLATFORM_MULTITHREADING_TATOMIC_HPP
#define XYO_PLATFORM_MULTITHREADING_TATOMIC_HPP

#ifndef XYO_PLATFORM_DEPENDENCY_HPP
#	include <XYO/Platform/Dependency.hpp>
#endif

namespace XYO::Platform::Multithreading {

	template <typename T>
	class TAtomic {
		protected:
#ifdef XYO_PLATFORM_MULTI_THREAD
			std::atomic<T> value;
#endif
#ifdef XYO_PLATFORM_SINGLE_THREAD
			T value;
#endif

		public:
#ifdef XYO_PLATFORM_MULTI_THREAD
			static const bool isAtomic = true;
#endif
#ifdef XYO_PLATFORM_SINGLE_THREAD
			static const bool isAtomic = false;
#endif

			inline TAtomic(){};

			inline void set(const T &value_) {
#ifdef XYO_PLATFORM_MULTI_THREAD
				value.store(value_, std::memory_order_relaxed);
#endif
#ifdef XYO_PLATFORM_SINGLE_THREAD
				value = value_;
#endif
			};

			inline void set(T &&value_) {
#ifdef XYO_PLATFORM_MULTI_THREAD
				value.store(value_, std::memory_order_relaxed);
#endif
#ifdef XYO_PLATFORM_SINGLE_THREAD
				value = std::move(value_);
#endif
			};

			inline T get() const {
#ifdef XYO_PLATFORM_MULTI_THREAD
				return value.load(std::memory_order_relaxed);
#endif
#ifdef XYO_PLATFORM_SINGLE_THREAD
				return value;
#endif
			};

			inline TAtomic(const TAtomic &value_) {
				set(value_.get());
			};

			inline TAtomic(TAtomic &&value_) {
				set(value_.get());
			};

			inline TAtomic &operator=(const TAtomic &value_) {
				set(value_.get());
			};

			inline TAtomic &operator=(TAtomic &&value_) {
				set(value_.get());
			};

			inline TAtomic(const T &value_) {
				set(value_);
			};

			inline TAtomic(T &&value_) {
				set(value_);
			};

			inline TAtomic &operator=(const T &value_) {
				set(value_);
				return *this;
			};

			inline TAtomic &operator=(T &&value_) {
				set(value_);
				return *this;
			};

			inline operator T() const {
				return get();
			};
	};
};

#endif
