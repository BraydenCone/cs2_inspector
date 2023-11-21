#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <iostream>

#include "sdk/sdk.h"
#include "sdk/memory.h"

int start_console(FILE* stream);
int stop_console(FILE* stream);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	FILE* stream = 0;

	start_console(stream);

	memory::module_info client_info;
	uint32_t process_id = memory::find_process(TEXT("cs2.exe"));
	HANDLE process_handle = OpenProcess(PROCESS_ALL_ACCESS, 0, process_id);
	memory::attatch((uintptr_t)process_handle);
	memory::get_module(&client_info, process_id, TEXT("client.dll"));
	sdk::initialize(client_info.base);

	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (GetAsyncKeyState(VK_DELETE))
		{
			break;
		}

		sdk::update();
	}

	stop_console(stream);

	return 0;
}

int start_console(FILE* stream)
{
	if (AllocConsole() == 0)
	{
		stop_console(stream);
		return 1;
	}
	if (freopen_s(&stream, "CONOUT$", "w", stdout))
	{
		stop_console(stream);
		return 1;
	}
	return 0;
}

int stop_console(FILE* stream)
{
	if (stream)
	{
		fclose(stream);
		stream = 0;
	}
	FreeConsole();
	return 0;
}
