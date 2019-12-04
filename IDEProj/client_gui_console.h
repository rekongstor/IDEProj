#pragma once
#include "field.hpp"

class client_gui_console
{
public:
	enum class state
	{
		preparation,
		turns,
		end
	};


	client_gui_console();
	void clear();
	void draw();
	state get_state();
	void set_state(state st);

private:
	state m_client_state;
	void draw_own_grid(field* my);
	void draw_game_grid(field* my, field* en);
};