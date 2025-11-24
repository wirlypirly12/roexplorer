#include "memory.h"

#define WIN32_LEAN_AND_MEAN
#include <TlHelp32.h>
#include <iostream>
RBXMemory::RBXMemory(std::string_view process_name)
{

	::PROCESSENTRY32 entry = {};
	entry.dwSize = sizeof(::PROCESSENTRY32);

	const HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	while (::Process32Next(snapshot, &entry))
	{
		if (!process_name.compare(entry.szExeFile))
		{
			id = entry.th32ProcessID;
			handle = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, id);
			break;
		}
	}

	if (snapshot)
		::CloseHandle(snapshot);

	base_address = get_module_address(process_name);
}

RBXMemory::~RBXMemory()
{
	if (handle)
		::CloseHandle(handle);
}

uintptr_t RBXMemory::get_module_address(std::string_view module_name) const
{
	::MODULEENTRY32 entry = {};
	entry.dwSize = sizeof(::MODULEENTRY32);

	const HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, id);

	uintptr_t result = 0;

	while (::Module32Next(snapshot, &entry))
	{
		if (!module_name.compare(entry.szModule))
		{
			result = reinterpret_cast<uintptr_t>(entry.modBaseAddr);
			break;
		}
	}

	if (snapshot)
		::CloseHandle(snapshot);

	return result;
}

uintptr_t RBXMemory::get_base() const
{
	return this->base_address;
}

std::string RBXMemory::read_string(uintptr_t address) const
{
	const auto length = read_memory<int>(address + 0x18);

	if (length >= 16u)
	{
		const auto new_address = read_memory<uintptr_t>(address);
		return f_read_string(new_address);
	}
	else
	{
		const auto name = f_read_string(address);
		return name;
	}
}

std::string RBXMemory::f_read_string(uintptr_t address) const
{
	std::string string;
	char character = 0;
	int char_size = sizeof(character);
	int offset = 0;

	string.reserve(204);

	while (offset < 200)
	{
		character = read_memory<uintptr_t>(address + offset);

		if (character == 0)
			break;

		offset += char_size;
		string.push_back(character);
	}

	return string;
}
