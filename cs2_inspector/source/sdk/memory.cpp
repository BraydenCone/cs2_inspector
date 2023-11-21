#include "memory.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <TlHelp32.h>
#include <memoryapi.h>

namespace memory
{
	namespace signature
	{
		uintptr_t start = 0;
		uintptr_t end = 0;
	}

	uintptr_t target = 0;
}

void memory::attatch(uintptr_t target)
{
	memory::target = target;
}

void memory::detach()
{
	memory::target = 0;
}

void memory::read(uintptr_t address, uintptr_t buffer, uint64_t size)
{
	if (ReadProcessMemory((HANDLE)target, (void*)address, (void*)buffer, size, 0) == 0)
	{
		memset((void*)buffer, 0, size);
	}
}

void memory::write(uintptr_t address, uintptr_t buffer, uint64_t size)
{
	if (WriteProcessMemory((HANDLE)target, (void*)address, (void*)buffer, size, 0) == 0)
	{
		memset((void*)buffer, 0, size);
	}
}

uint32_t memory::find_process(const wchar_t* name)
{
	DWORD id = 0;
	HANDLE snap = 0;
	PROCESSENTRY32 entry;

	if (!name)
	{
		return 0;
	}

	snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (snap != INVALID_HANDLE_VALUE)
	{
		entry.dwSize = sizeof(entry);

		if (Process32First(snap, &entry))
		{
			do
			{
				if (!_wcsicmp(entry.szExeFile, name))
				{
					id = entry.th32ProcessID;
					break;
				}
			} while (Process32Next(snap, &entry));
		}
	}

	CloseHandle(snap);

	return (uint32_t)id;
}

int memory::get_module(memory::module_info* info, uint32_t process_id, const wchar_t* name)
{
	HANDLE snap = 0;
	MODULEENTRY32 entry;

	snap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, process_id);
	if (snap != INVALID_HANDLE_VALUE)
	{
		entry.dwSize = sizeof(entry);

		if (Module32First(snap, &entry))
		{
			do
			{
				if (!_wcsicmp(entry.szModule, name))
				{
					info->base = (uintptr_t)entry.modBaseAddr;
					info->size = (uint64_t)entry.modBaseSize;
					CloseHandle(snap);
					return 0;
				}
			} while (Module32Next(snap, &entry));
		}
	}
	CloseHandle(snap);
	return 1;
}
