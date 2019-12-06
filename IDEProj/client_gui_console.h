#pragma once
#include "field.hpp"
/// <summary>
/// Консольный GUI для игры
/// </summary>
class client_gui_console
{
public:
	/// <summary>
	/// Энумератор состояний. Почти повторяет собой серверную реализацию, но различает свой и чужой ход
	/// </summary>
	enum class egs
	{
		preparation,
		my_turn,
		enemy_turn,
		end
	};

	/// <summary>
	/// Конструктор класса GUI
	/// </summary>
	/// <param name="my">Указатель на своё поле</param>
	/// <param name="en">Указатель на поле противника</param>
	client_gui_console (field* my, field* en);
	/// <summary>
	/// Метод отрисовки
	/// </summary>
	void draw();
	/// <summary>
	/// Установка состояния игры
	/// </summary>
	/// <param name="st">Энумератор состояния</param>
	void set_state(egs st);
	/// <summary>
	/// Функция, возвращающая текущее состояние
	/// </summary>
	/// <returns></returns>
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