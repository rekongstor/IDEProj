#pragma once

#include "participant.h"

class game
{
	enum game_states
	{
		preparation,
		turn_1,
		turn_2,
		end
	};
public:
	game(participant&, participant&);
	bool place(int, int);
	void set_state(game_states);
	bool turn(int, int);

private:
	participant& m_participant_1;
	participant& m_participant_2;
	int m_field_1[10][10];
	int m_field_2[10][10];
};