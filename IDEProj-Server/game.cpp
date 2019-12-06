#include "game.h"

game::game() : m_participant_1(nullptr), m_participant_2(nullptr), m_game_state(egs::preparation)
{
}

bool game::place(int, int)
{
	return false;
}

void game::set_state(egs gs)
{
	m_game_state = gs;
}

game::egs game::get_state()
{
	return m_game_state;
}

bool game::turn(int x, int y)
{
	field k;

	k.set_cell(x, y, k.get_cell(x, y) | shot);

	return false;
}