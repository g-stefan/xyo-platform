// Platform
// Copyright (c) 2024-2025 Grigore Stefan <g_stefan@yahoo.com>
// MIT License (MIT) <http://opensource.org/licenses/MIT>
// SPDX-FileCopyrightText: 2024-2025 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: MIT

#include <XYO/Platform/Copyright.hpp>
#include <XYO/Platform/Copyright.rh>

namespace XYO::Platform::Copyright {

	static const char *copyright_ = XYO_PLATFORM_COPYRIGHT;
	static const char *publisher_ = XYO_PLATFORM_PUBLISHER;
	static const char *company_ = XYO_PLATFORM_COMPANY;
	static const char *contact_ = XYO_PLATFORM_CONTACT;

	const char *copyright() {
		return copyright_;
	};

	const char *publisher() {
		return publisher_;
	};

	const char *company() {
		return company_;
	};

	const char *contact() {
		return contact_;
	};

};
