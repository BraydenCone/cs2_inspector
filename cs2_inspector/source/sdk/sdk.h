#pragma once

#include <inttypes.h>

namespace cs2
{
#define MAX_NAME 32
#define MAX_PLAYERS 64

	struct vec2
	{
		float x, y;
	};

	struct vec3
	{
		float x, y, z;
	};

	struct rotator
	{
		float pitch, yaw;
	};

	struct c_controller
	{
		int alive;
		char name[MAX_NAME];

		void update(uintptr_t controller_address);
	};

	struct c_pawn
	{
		vec3 position;
		vec3 camera_position;
		vec2 view_angle;
		int max_health;
		int health;
		int team_id;

		void update(uintptr_t pawn_address);
	};

	struct c_player
	{
		c_controller controller;
		c_pawn pawn;

		void update(uintptr_t controller_address, uintptr_t pawn_address);
	};

	struct c_game
	{
		c_player local_player;
		c_player players[MAX_PLAYERS];
		uint64_t number_players;

		void update();
	};
}

namespace sdk
{
	void initialize(uintptr_t client);

	cs2::c_game* get_game();
	void update();
}
