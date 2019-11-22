#include "game.h"

game::game(participant& mp1, participant& mp2) : m_participant_1(mp1), m_participant_2(mp2), game_state(egs::preparation)
{
	for (auto& i : m_field_1)
		for (auto& j : i)
			j = 0;

	for (auto& i : m_field_2)
		for (auto& j : i)
			j = 0;
}

bool game::place(int, int)
{
	return false;
}

void game::set_state(egs gs)
{

}

bool game::turn(int, int)
{
	return false;
}