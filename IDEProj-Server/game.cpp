#include "game.h"

game::game(participant& mp1, participant& mp2) : m_participant_1(mp1), m_participant_2(mp2), m_field_1(), m_field_2(), game_state(preparation)
{

}

bool game::place(int, int)
{
	return false;
}

void game::set_state(egs gs)
{

}

bool game::turn(int x, int y)
{
	field k;

	k.set_cell(x, y, k.get_cell(x, y) | shot);

	return false;
}