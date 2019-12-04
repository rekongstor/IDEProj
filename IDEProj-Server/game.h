#pragma once

#include "field.hpp"

class game
{
public:
	enum class egs
	{
		preparation,
		turn_1,
		turn_2,
		end
	};
	game();
	bool place(int, int);
	void set_state(egs);
	egs get_state();
	bool turn(int, int);

	void* m_participant_1;
	void* m_participant_2;
	field m_field_1;
	field m_field_2;
	egs m_game_state;
	bool m_p1_ready;
	bool m_p2_ready;
private:
};