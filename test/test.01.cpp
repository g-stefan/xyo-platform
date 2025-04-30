// Created by Grigore Stefan <g_stefan@yahoo.com>
// Public domain (Unlicense) <http://unlicense.org>
// SPDX-FileCopyrightText: 2024-2025 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: Unlicense

#include <XYO/Platform.hpp>

using namespace XYO::Platform;

void test() {
	printf("Platform name %s\n", XYO_PLATFORM_NAME);
};

int main(int cmdN, char *cmdS[]) {
	try {

		test();

		return 0;

	} catch (const std::exception &e) {
		printf("* Error: %s\n", e.what());
	} catch (...) {
		printf("* Error: Unknown\n");
	};

	return 1;
};
