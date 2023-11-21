#pragma once

#include <inttypes.h>

namespace memory
{
	struct module_info
	{
		uintptr_t base;
		uint64_t size;
	};

	void attatch(uintptr_t target);
	void detach();

	void read(uintptr_t address, uintptr_t buffer, uint64_t size);
	void write(uintptr_t address, uintptr_t buffer, uint64_t size);

	uint32_t find_process(const wchar_t* name);
	int get_module(module_info* info, uint32_t process_id, const wchar_t* name);
}
