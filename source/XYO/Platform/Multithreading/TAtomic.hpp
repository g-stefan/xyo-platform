// Platform
// Copyright (c) 2016-2026 Grigore Stefan <g_stefan@yahoo.com>
// MIT License (MIT) <http://opensource.org/licenses/MIT>
// SPDX-FileCopyrightText: 2016-2026 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: MIT

#ifndef XYO_PLATFORM_MULTITHREADING_TATOMIC_HPP
#define XYO_PLATFORM_MULTITHREADING_TATOMIC_HPP

#ifndef XYO_PLATFORM_DEPENDENCY_HPP
#	include <XYO/Platform/Dependency.hpp>
#endif

namespace XYO::Platform::Multithreading {

	// Default memory ordering
	//   get()              - acquire: sees everything written before the matching set()
	//   set()              - release: publishes everything written before it
	//   read-modify-write  - acq_rel
	// On x86/x64 acquire/release compile to plain loads/stores (no extra cost),
	// on ARM they become LDAR/STLR (ARMv8) or DMB (ARMv7), required for correct publication.
	// Pass std::memory_order_relaxed explicitly only for values that publish nothing (counters, statistics).
	// Valid orders: set - relaxed/release/seq_cst, get - relaxed/acquire/seq_cst.
	// In single thread mode the memory order is ignored.

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
			static constexpr bool isAlwaysLockFree = std::atomic<T>::is_always_lock_free;
#endif
#ifdef XYO_PLATFORM_SINGLE_THREAD
			static const bool isAtomic = false;
			static constexpr bool isAlwaysLockFree = true;
#endif

			// constexpr allows constant initialization of static instances,
			// avoiding static initialization order issues
			inline constexpr TAtomic() noexcept : value() {};

			inline constexpr TAtomic(const T &value_) noexcept : value(value_) {};

			inline TAtomic(const TAtomic &value_) noexcept : value(value_.get()) {};

			inline void set(const T &value_, std::memory_order order = std::memory_order_release) noexcept {
#ifdef XYO_PLATFORM_MULTI_THREAD
				value.store(value_, order);
#endif
#ifdef XYO_PLATFORM_SINGLE_THREAD
				(void)order;
				value = value_;
#endif
			};

			inline T get(std::memory_order order = std::memory_order_acquire) const noexcept {
#ifdef XYO_PLATFORM_MULTI_THREAD
				return value.load(order);
#endif
#ifdef XYO_PLATFORM_SINGLE_THREAD
				(void)order;
				return value;
#endif
			};

			// Set new value, return previous value
			inline T exchange(const T &value_, std::memory_order order = std::memory_order_acq_rel) noexcept {
#ifdef XYO_PLATFORM_MULTI_THREAD
				return value.exchange(value_, order);
#endif
#ifdef XYO_PLATFORM_SINGLE_THREAD
				(void)order;
				T retV = value;
				value = value_;
				return retV;
#endif
			};

			// If value == expected set value to desired and return true,
			// else load current value into expected and return false
			inline bool compareExchange(T &expected, const T &desired, std::memory_order order = std::memory_order_acq_rel) noexcept {
#ifdef XYO_PLATFORM_MULTI_THREAD
				return value.compare_exchange_strong(expected, desired, order);
#endif
#ifdef XYO_PLATFORM_SINGLE_THREAD
				(void)order;
				if (value == expected) {
					value = desired;
					return true;
				};
				expected = value;
				return false;
#endif
			};

			// Like compareExchange but may fail spuriously, faster inside a retry loop on ARM (LL/SC)
			inline bool compareExchangeWeak(T &expected, const T &desired, std::memory_order order = std::memory_order_acq_rel) noexcept {
#ifdef XYO_PLATFORM_MULTI_THREAD
				return value.compare_exchange_weak(expected, desired, order);
#endif
#ifdef XYO_PLATFORM_SINGLE_THREAD
				return compareExchange(expected, desired, order);
#endif
			};

			// Integral and pointer types only, return previous value
			template <typename U>
			inline T fetchAdd(U value_, std::memory_order order = std::memory_order_acq_rel) noexcept {
#ifdef XYO_PLATFORM_MULTI_THREAD
				return value.fetch_add(value_, order);
#endif
#ifdef XYO_PLATFORM_SINGLE_THREAD
				(void)order;
				T retV = value;
				value += value_;
				return retV;
#endif
			};

			template <typename U>
			inline T fetchSub(U value_, std::memory_order order = std::memory_order_acq_rel) noexcept {
#ifdef XYO_PLATFORM_MULTI_THREAD
				return value.fetch_sub(value_, order);
#endif
#ifdef XYO_PLATFORM_SINGLE_THREAD
				(void)order;
				T retV = value;
				value -= value_;
				return retV;
#endif
			};

			inline TAtomic &operator=(const TAtomic &value_) noexcept {
				set(value_.get());
				return *this;
			};

			inline TAtomic &operator=(const T &value_) noexcept {
				set(value_);
				return *this;
			};

			inline operator T() const noexcept {
				return get();
			};
	};
};

#endif
