#pragma once

class Client;

/// <summary>
/// Консольный GUI для игры
/// </summary>
class ConsoleGui
{
public:

	/// <summary>
	/// Конструктор класса GUI
	/// </summary>
	/// <param name="my">Указатель на своё поле</param>
	/// <param name="en">Указатель на поле противника</param>
	ConsoleGui (Client* cl);
	/// <summary>
	/// Метод отрисовки
	/// </summary>
	void draw();

private:
	Client* client;
	void draw_own_grid();
	void draw_game_grid();
	void draw_field_cell(int c);
	void clear();
};