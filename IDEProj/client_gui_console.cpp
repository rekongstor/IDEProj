#include "client_gui_console.h"
#include <iostream>

using namespace std;

client_gui_console::client_gui_console(field* my, field* en)
{
	m_my = my;
	m_en = en;
	m_client_state = egs::preparation;
}

void client_gui_console::clear()
{
	system("clear");
}

void client_gui_console::draw()
{
	clear();
	if (m_client_state == egs::preparation)
	{

		//printf("\x1B[31mTexting\033[0m\t\t");
		cout << "\x1B[92m**********************\n    SET YOUR SHIPS    \n**********************\033[0m" << endl;
		draw_own_grid();
		cout << "\x1B[92mEnter ship location in the following format:\n\x1B[91ms x y w d\n\x1B[92mWhere\n\x1B[91mx \x1B[92m- x coordinate [0-9]\n\x1B[91my \x1B[92m- y coordinate [0-9]\n\x1B[91mw \x1B[92m- width of the ship [1-4]\n\x1B[91md \x1B[92m- direction of the ship [v/h]\n\033[0m";
	}
	if (m_client_state == egs::my_turn)
	{
		cout << "\x1B[92m**********************" << endl << "     TAKE A SHOT!     " << endl << "**********************\x1B[0m" << endl;
		draw_game_grid();
		cout << "\x1B[92mEnter where to shoot in the following format:\n\x1B[91mx y\n\x1B[92mWhere\n\x1B[91mx \x1B[92m- x coordinate [0-9]\n\x1B[91my \x1B[92m- y coordinate [0-9]\x1B[0m\n";
	}
	if (m_client_state == egs::enemy_turn)
	{
		cout << "\x1B[92m**********************" << endl << " WAIT FOR YOUR ENEMY! " << endl << "**********************\x1B[0m" << endl;
		draw_game_grid();
		//cout << "Enter ship location in the following format:\ns x y w d\nWhere\nx - x coordinate [0-9]\ny - y coordinate [0-9]\nw - width of the ship [1-4]\nd - direction of the ship [v/h]\n";
	}
	if (m_client_state == egs::end)
	{
		cout << "\x1B[92m**********************" << endl << "   THE GAME IS OVER   " << endl << "**********************\x1B[0m" << endl;
		draw_game_grid();
		//cout << "Enter ship location in the following format:\ns x y w d\nWhere\nx - x coordinate [0-9]\ny - y coordinate [0-9]\nw - width of the ship [1-4]\nd - direction of the ship [v/h]\n";
	}
}

void client_gui_console::set_state(egs st)
{
	m_client_state = st;
}

void client_gui_console::draw_field_cell(int c)
{
	if (c == 0)
		cout << "\x1B[90m°" << ' ';
	if (c == 1)
		cout << "\x1B[37m•" << ' ';
	if (c == 2)
		cout << "\x1B[96m*" << ' ';
	if (c == 3)
		cout << "\x1B[91m•" << ' ';
}

void client_gui_console::draw_game_grid()
{
	cout << "\x1B[95m  x 0 1 2 3 4 5 6 7 8 9			  x 0 1 2 3 4 5 6 7 8 9\x1B[0m\n\x1B[92my   \x1B[95m___________________			\x1B[92my   \x1B[95m___________________\n";
	for (int y = 0; y < 10; ++y)
	{
		cout << "\x1B[92m" << y << "|  \x1B[0m";
		for (int x = 0; x < 10; ++x)
			draw_field_cell(m_my->get_cell(x, y));

		cout << "		";

		cout << "\x1B[92m" << y << "|  \x1B[0m";
		for (int x = 0; x < 10; ++x)
			draw_field_cell(m_en->get_cell(x, y));
		cout << endl;
	}
	cout << endl;
}

void client_gui_console::draw_own_grid()
{
	cout << "\x1B[95m  x 0 1 2 3 4 5 6 7 8 9\n\x1B[92my   \x1B[95m___________________\n";
	for (int y = 0; y < 10; ++y)
	{
		cout << "\x1B[92m" << y << "|  \x1B[0m";
		for (int x = 0; x < 10; ++x)
			draw_field_cell(m_my->get_cell(x, y));
		cout << endl;
	}
	cout << endl;
}

