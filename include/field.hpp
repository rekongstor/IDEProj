#pragma once
/// <summary>
/// Энумератор-битовая маска для клетки поля
/// </summary>
enum cell
{
	ship = 1 << 0,
	shot = 1 << 1
};

/// <summary>
/// Класс игрового поля
/// </summary>
class field
{
public:
	/// <summary>
	/// Конструктор создания игрового поля
	/// </summary>
	field(): m_1(0), m_2(0), m_3(0), m_4(0)
	{
		clear();
	}
	/// <summary>
	/// Функция, возвращающся значение клетки поля
	/// </summary>
	const int& get_cell(int x, int y)
	{
		if ((x < 0) || (x > 9) || (y < 0) || (y > 9))
			return m_field[0][0];
		return m_field[x][y];
	}
	/// <summary>
	/// Функция, устанавливающая значение клетки поля. Возвращает true в случае успеха
	/// </summary>
	bool set_cell(int x , int y, int val)
	{
		if ((x < 0) || (x > 9) || (y < 0) || (y > 9))
			return false;
		m_field[x][y] = val;
		return true;
	}
	/// <summary>
	/// Метод для очистки поля
	/// </summary>
	void clear()
	{
		for (auto& i : m_field)
			for (auto& j : i)
				j = 0;
	}

	/// <summary>
	/// Функция, возвращающся true, если координата не выходит за пределы поля
	/// </summary>
	bool check_bound(int x, int y)
	{
		return (x >= 0 && x <= 9 && y >= 0 && y <= 9);
	};
	/// <summary>
	/// Функция, возвращающся true, если корабль по данным координатам убит
	/// </summary>
	bool check_kill(int x, int y)
	{
		// find ship start
		for (int i = 1; i <= 4; ++i)
		{
			if (m_field[x - 1][y] & cell::ship)
				--x;
			if (m_field[x][y - 1] & cell::ship)
				--y;
		}

		//check kill
		for (int i = 1; i <= 4; ++i)
		{
			if (m_field[x][y] & cell::ship)
			{
				if (!(m_field[x][y] & cell::shot))
					return false;

				if (m_field[x + 1][y] & cell::ship)
					++x;
				if (m_field[x][y + 1] & cell::ship)
					++y;
			}
			else
				return true;
		}
	}
	/// <summary>
	/// Метод, устанавливающий вокруг убитого корабля промахи
	/// </summary>
	void kill(int x, int y)
	{
		// find ship start
		for (int i = 1; i <= 4; ++i)
		{
			if (m_field[x - 1][y] & cell::ship)
				--x;
			if (m_field[x][y - 1] & cell::ship)
				--y;
		}

		for (int i = 1; i <= 4; ++i)
		{
			if (m_field[x][y] & cell::ship)
			{
				if (check_bound(x - 1, y - 1))
					m_field[x - 1][y - 1] |= cell::shot;
				if (check_bound(x - 0, y - 1))
					m_field[x - 0][y - 1] |= cell::shot;
				if (check_bound(x + 1, y - 1))
					m_field[x + 1][y - 1] |= cell::shot;

				if (check_bound(x - 1, y - 0))
					m_field[x - 1][y - 0] |= cell::shot;
				if (check_bound(x + 1, y - 0))
					m_field[x + 1][y - 0] |= cell::shot;

				if (check_bound(x - 1, y + 1))
					m_field[x - 1][y + 1] |= cell::shot;
				if (check_bound(x - 0, y + 1))
					m_field[x - 0][y + 1] |= cell::shot;
				if (check_bound(x + 1, y + 1))
					m_field[x + 1][y + 1] |= cell::shot;

				if (m_field[x + 1][y] & cell::ship)
					++x;
				if (m_field[x][y + 1] & cell::ship)
					++y;
			}
			else
				return;
		}
	}
	/// <summary>
	/// Функция, возвращающая true в случае, если расставлены все корабли на поле. В Debug конфигурации всегда возвращает true
	/// </summary>
	bool ships_ready()
	{
#ifdef NDEBUG
		return true;
#endif
		return (m_1 == 4 && m_2 == 3 && m_3 == 2 && m_4 == 1);
	}
	/// <summary>
	/// Функция, устанавливающая корабль по заданным координатам, направлению и длине корабля. Возвращает true в случае успеха
	/// </summary>
	bool set_ship(int x, int y, int length, char direction)
	{
		if ((direction != 'h') && (direction != 'v'))
			return false;



		if ((length < 1 || length > 4) ||
			(direction == 'v' && y + length - 1 > 9) ||
			(direction == 'h' && x + length - 1 > 9))
		{
			return false;
		} // length check

		for (int i = 0; i < length; ++i)
			if (!check_bound(x + i * (direction == 'h'), y + i * (direction == 'v')))
				return false; // bound check

		for (int i = 0; i < length; ++i) 
		{ //-1 and +1 for corners

			if (check_bound(x + i * (direction == 'h'), y + i * (direction == 'v'))) // to be sure if we're in m_field boundaries
				if (m_field[x + i * (direction == 'h')][y + i * (direction == 'v')])
					return false;

			if (check_bound(x + i * (direction == 'h') + 1, y + i * (direction == 'v')))
				if (m_field[x + i * (direction == 'h') + 1][y + i * (direction == 'v')])
					return false;

			if (check_bound(x + i * (direction == 'h') - 1, y + i * (direction == 'v')))
				if (m_field[x + i * (direction == 'h') - 1][y + i * (direction == 'v')])
					return false;

			if (check_bound(x + i * (direction == 'h'), y + i * (direction == 'v') + 1))
				if (m_field[x + i * (direction == 'h')][y + i * (direction == 'v') + 1])
					return false;

			if (check_bound(x + i * (direction == 'h'), y + i * (direction == 'v') - 1))
				if (m_field[x + i * (direction == 'h')][y + i * (direction == 'v') - 1])
					return false;

		} // cross check


		if (check_bound(x + 0 * (direction == 'h') - 1, y + 0 * (direction == 'v') - 1))
			if (m_field[x + 0 * (direction == 'h') - 1][y + 0 * (direction == 'v') - 1])
				return false;
		if (check_bound(x + 0 * (direction == 'h') - 1, y + 0 * (direction == 'v') + 1))
			if (m_field[x + 0 * (direction == 'h') - 1][y + 0 * (direction == 'v') + 1])
				return false;
		if (check_bound(x + 0 * (direction == 'h') + 1, y + 0 * (direction == 'v') - 1))
			if (m_field[x + 0 * (direction == 'h') + 1][y + 0 * (direction == 'v') - 1])
				return false;
		if (check_bound(x + 0 * (direction == 'h') + 1, y + 0 * (direction == 'v') + 1))
			if (m_field[x + 0 * (direction == 'h') + 1][y + 0 * (direction == 'v') + 1])
				return false;

		if (check_bound(x + (length - 1) * (direction == 'h') - 1, y + (length - 1) * (direction == 'v') - 1))
			if (m_field[x + (length - 1) * (direction == 'h') - 1][y + (length - 1) * (direction == 'v') - 1])
				return false;
		if (check_bound(x + (length - 1) * (direction == 'h') - 1, y + (length - 1) * (direction == 'v') + 1))
			if (m_field[x + (length - 1) * (direction == 'h') - 1][y + (length - 1) * (direction == 'v') + 1])
				return false;
		if (check_bound(x + (length - 1) * (direction == 'h') + 1, y + (length - 1) * (direction == 'v') - 1))
			if (m_field[x + (length - 1) * (direction == 'h') + 1][y + (length - 1) * (direction == 'v') - 1])
				return false;
		if (check_bound(x + (length - 1) * (direction == 'h') + 1, y + (length - 1) * (direction == 'v') + 1))
			if (m_field[x + (length - 1) * (direction == 'h') + 1][y + (length - 1) * (direction == 'v') + 1])
				return false;
		// check corners

		switch (length)
		{
		case 1:
			if (m_1 == 4)
				return false;
			else
				++m_1;
			break;
		case 2:
			if (m_2 == 3)
				return false;
			else
				++m_2;
			break;
		case 3:
			if (m_3 == 2)
				return false;
			else
				++m_3;
			break;
		case 4:
			if (m_4 == 1)
				return false;
			else
				++m_4;
			break;
		default:
			break;
		}

		for (int i = 0; i < length; ++i)
		{
			set_cell(x + i * (direction == 'h'), y + i * (direction == 'v'), cell::ship);
		}
		return true;
	}

private:
	int m_field[10][10];
	int m_1, m_2, m_3, m_4;
};
