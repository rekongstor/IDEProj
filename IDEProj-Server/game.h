#pragma once

#include "field.hpp"
/// <summary>
/// Класс с игровой логикой
/// </summary>
class Game
{
public:
	/// <summary>
	/// Энумератор, состоящий из состояний игры
	/// </summary>
	enum class egs
	{
		preparation,
		turn_1,
		turn_2,
		end
	};
	/// <summary>
	/// Конструктор по умолчанию
	/// </summary>
	Game();
	/// <summary>
	/// Метод установки состояния
	/// </summary>
	/// <param name="">Энумератор состояния</param>
	void set_state(egs);
	/// <summary>
	/// Функция, возвращающая текущее состояние
	/// </summary>
	/// <returns></returns>
	egs get_state();
	/// <summary>
	/// Поле первого игрока
	/// </summary>
	field m_field_1;
	/// <summary>
	/// Поле второго игрока
	/// </summary>
	field m_field_2;
	/// <summary>
	/// Игровое состояние
	/// </summary>
	egs m_game_state;
	/// <summary>
	/// Готовность первого игрока
	/// </summary>
	bool m_p1_ready;
	/// <summary>
	/// Готовность второго игрока
	/// </summary>
	bool m_p2_ready;
private:
};