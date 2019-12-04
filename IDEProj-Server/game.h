#pragma once

#include "participant.h"
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
	bool turn(int, int);

private:
	participant* m_participant_1;
	participant* m_participant_2;
	field m_field_1;
	field m_field_2;
	egs game_state;
};