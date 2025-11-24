#include "../class/memory.h"
#include "../class/instance.h"
#include "../ext/window/window.hpp"
#include <thread>
#include <iostream>

#include "../offsets/offsets.h"

int main() 
{
	std::unique_ptr<RBXMemory> roblox_memory = std::make_unique<RBXMemory>();
	SetConsoleTitle("explorer");

	if (roblox_memory->id == 0 || roblox_memory->base_address == 0)
	{
		std::cout << "Memory not initlized properly" << std::endl;
		return -9;
	}

	auto base = roblox_memory->get_base();

	std::cout << "PID: " << roblox_memory->id << std::endl;
	std::cout << "Base address -> 0x" << std::hex << base << std::dec << std::endl;

	Overlay ovr;

	ovr.SetupOverlay("roexplorer");

	bool set_pos = false;

	while (ovr.shouldRun)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));

		ovr.StartRender();

		if (ovr.RenderMenu) {

			if (!set_pos)
				ImGui::SetNextWindowSize({ 800, 1000 });

			set_pos = true;

			ovr.Render();
		}
			

		ovr.EndRender();
	}
}