// Created by Grigore Stefan <g_stefan@yahoo.com>
// Public domain (Unlicense) <http://unlicense.org>
// SPDX-FileCopyrightText: 2024-2025 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: Unlicense

messageAction("make.config");

Shell.mkdirRecursivelyIfNotExists("output");
Shell.mkdirRecursivelyIfNotExists("temp");

// ---
var defines = [];

if (OS.isWindows()) {
	defines = defines.concat([
		"XYO_PLATFORM_OS_WINDOWS"
	]);

	if (Platform.name.indexOf("msvc") >= 0) {
		defines = defines.concat([
			"XYO_PLATFORM_COMPILER_MSVC"
		]);
	};

	if (Platform.name.indexOf("mingw") >= 0) {
		defines = defines.concat([
			"XYO_PLATFORM_COMPILER_GCC",
			"XYO_PLATFORM_OS_MINGW"
		]);
	};
};

if (OS.isLinux()) {
	defines = defines.concat([
		"XYO_PLATFORM_OS_LINUX",
		"XYO_PLATFORM_COMPILER_GCC"
	]);
};

defines = defines.concat([
	"XYO_PLATFORM_MULTI_THREAD",
	"XYO_PLATFORM_NAME=\"" + Platform.name + "\""
]);

if (Platform.machine == "x64") {
	defines[defines.length] = "XYO_PLATFORM_64BIT";
};

if (Platform.machine == "x86") {
	defines[defines.length] = "XYO_PLATFORM_32BIT";
};

compileAndRunTemp({
	project : Project.name + ".config",
	type : "exe",
	defines : defines,
	includePath : [ "source" ],
	cppSource : [ "source/" + Project.sourcePath[0] + ".Config.cpp" ],
	crt : "static"
});
