// Platform
// Copyright (c) 2024-2025 Grigore Stefan <g_stefan@yahoo.com>
// MIT License (MIT) <http://opensource.org/licenses/MIT>
// SPDX-FileCopyrightText: 2024-2025 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: MIT

#include <XYO/Platform/License.hpp>
#include <XYO/Platform/Copyright.hpp>

namespace XYO::Platform::License {

	static const char *licenseMITShort_ = "MIT License (MIT) <http://opensource.org/licenses/MIT>\r\n";

	static const char *licenseMITHeader_ =
	    "        MIT License\r\n"
	    "\r\n";

	static const char *licenseMITContent_ =
	    "\r\n"
	    "Permission is hereby granted, free of charge, to any person obtaining a copy\r\n"
	    "of this software and associated documentation files (the \" Software \"), to deal\r\n"
	    "in the Software without restriction, including without limitation the rights\r\n"
	    "to use, copy, modify, merge, publish, distribute, sublicense, and/or sell\r\n"
	    "copies of the Software, and to permit persons to whom the Software is\r\n"
	    "furnished to do so, subject to the following conditions:\r\n"
	    "\r\n"
	    "The above copyright notice and this permission notice shall be included in all\r\n"
	    "copies or substantial portions of the Software.\r\n"
	    "\r\n"
	    "THE SOFTWARE IS PROVIDED \" AS IS \", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR\r\n"
	    "IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,\r\n"
	    "FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE\r\n"
	    "AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER\r\n"
	    "LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,\r\n"
	    "OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE\r\n"
	    "SOFTWARE.\r\n";

	std::string license() {
		std::string retV;
		retV += licenseMITHeader();
		retV += Copyright::copyright();
		retV += "\r\n";
		retV += licenseMITContent();
		return retV;
	};

	std::string shortLicense() {
		std::string retV;
		retV += Copyright::copyright();
		retV += "\r\n";
		retV += licenseMITShort();
		return retV;
	};

	std::string licenseMITShort() {
		return licenseMITShort_;
	};

	std::string licenseMITHeader() {
		return licenseMITHeader_;
	};

	std::string licenseMITContent() {
		return licenseMITContent_;
	};

};
