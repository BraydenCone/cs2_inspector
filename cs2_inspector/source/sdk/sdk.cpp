#include "sdk.h"

#include <cstring>

#include "memory.h"

static cs2::c_game g_game;

namespace client_address
{
	uintptr_t global_vars = 0;
	uintptr_t entity_list = 0;
	uintptr_t matrix = 0;
	uintptr_t view_angle = 0;
	uintptr_t local_player_controller = 0;
	uintptr_t local_player_pawn = 0;
}

void cs2::c_controller::update(uintptr_t controller_address)
{
	memory::read(controller_address + 0x7C4, (uintptr_t)&alive, sizeof(c_controller::alive));
	char name_buffer[MAX_NAME];
	memory::read(controller_address + 0x610, (uintptr_t)name_buffer, MAX_NAME);
	memcpy(name, name_buffer, MAX_NAME);
}

void cs2::c_pawn::update(uintptr_t pawn_address)
{
	memory::read(pawn_address + 0x1214, (uintptr_t)&position, sizeof(c_pawn::position));
	memory::read(pawn_address + 0x1274, (uintptr_t)&camera_position, sizeof(c_pawn::camera_position));
	memory::read(pawn_address + 0x14F8, (uintptr_t)&view_angle, sizeof(c_pawn::view_angle));
	memory::read(pawn_address + 0x0328, (uintptr_t)&max_health, sizeof(c_pawn::max_health));
	memory::read(pawn_address + 0x032C, (uintptr_t)&health, sizeof(c_pawn::health));
	memory::read(pawn_address + 0x03BF, (uintptr_t)&team_id, sizeof(c_pawn::team_id));
}

void cs2::c_player::update(uintptr_t controller_address, uintptr_t pawn_address)
{
	pawn.update(pawn_address);
	controller.update(controller_address);
}

void cs2::c_game::update()
{
	uintptr_t player_list_entry = client_address::entity_list;
	uint64_t controller_address = 0;
	uint64_t player_count = 0;

	/* update local player */
	local_player.update(client_address::local_player_controller, client_address::local_player_pawn);

	memory::read(player_list_entry, (uintptr_t)&player_list_entry, sizeof(player_list_entry));
	memory::read(player_list_entry + 0x10, (uintptr_t)&player_list_entry, sizeof(player_list_entry));
	for (uint64_t i = 0; i < MAX_PLAYERS; i++)
	{
		//memory::read(player_list_entry + (i + 1) * 0x78, &controller_address, sizeof(controller_address));
		if (!controller_address || controller_address == client_address::local_player_controller)
		{
			continue;
		}

		c_player* player = &players[player_count];

		player->controller.update(controller_address);

		// get pawn info
		{
			uintptr_t pawn_list_entry = client_address::entity_list;
			uint64_t pawn_address = 0;
			uint32_t pawn = 0;

			memory::read(controller_address + 0x7BC, (uintptr_t)pawn, sizeof(pawn));
			memory::read(pawn_list_entry, (uintptr_t)&pawn_list_entry, sizeof(pawn_list_entry));
			memory::read(pawn_list_entry + 0x10 + 0x8 * ((pawn & 0x7FFF) >> 0x9), (uintptr_t)&pawn_list_entry, sizeof(pawn_list_entry));
			memory::read(pawn_list_entry + 0x78 * (pawn & 0x1FF), (uintptr_t)&pawn_address, sizeof(pawn_address));
			player->pawn.update(pawn_address);
		}
		player_count++;
	}
	number_players = player_count;
}

void sdk::initialize(uintptr_t client)
{
	client_address::global_vars = client + 0x169AFE0;
	client_address::entity_list = client + 0x17ADAF0;
	client_address::matrix = client + 0x180C0F0;
	client_address::view_angle = client + 0x186B100;
	memory::read(client + 0x16B9378, (uintptr_t)&client_address::local_player_controller, sizeof(client_address::local_player_controller));
	memory::read(client + 0x16B7328, (uintptr_t)&client_address::local_player_pawn, sizeof(client_address::local_player_pawn));
}

cs2::c_game* sdk::get_game()
{
	return &g_game;
}

void sdk::update()
{
	sdk::get_game()->update();
}
