/*
MIT License

Copyright (c) 2022 W4tev3n

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once

#include <Windows.h>

#include <cstdint>
#include <string>

namespace HWID
{
	// Request a hash of the last property from WMI.
	uint32_t WMIProperyHash(LPCWSTR Server, LPCWSTR Request, LPCWSTR Property);

	// Request processor manufacturer and features hash.
	uint32_t CPU_Id();
	// Request physical drives serial numbers hash.
	uint32_t Disk_Id();
	// Request video adapter descriptions hash.
	uint32_t Video_Id();
	// Request network adapters mac addresses hash.
	uint32_t Adapter_Id();
	// Request Windows GUID and profile name hash.
	uint32_t OS_Id();
	// Request processor identifiers from the environment.
	uint32_t CPU_Env();

	// Generate INI string, with current hardware fingerprint.
	std::string Fingerprint();
}