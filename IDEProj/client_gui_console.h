#pragma once
#include "field.hpp"

class client_gui_console
{
public:
	enum class egs
	{
		preparation,
		my_turn,
		enemy_turn,
		end
	};


	client_gui_console (field* my, field* en);
	void draw();
	void set_state(egs st);
	egs get_state();

private:
	egs m_client_state;
	field* m_my;
	field* m_en;
	void draw_own_grid();
	void draw_game_grid();
	void draw_field_cell(int c);
	void clear();
};