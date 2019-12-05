#pragma once

enum cell
{
	ship = 1 << 0,
	shot = 1 << 1
};

class field
{
public:
	field(): m_1(0), m_2(0), m_3(0), m_4(0)
	{
		clear();
	}
	const int& get_cell(int x, int y)
	{
		if ((x < 0) || (x > 9) || (y < 0) || (y > 9))
			return m_field[0][0];
		return m_field[x][y];
	}
	bool set_cell(int x , int y, int val)
	{
		if ((x < 0) || (x > 9) || (y < 0) || (y > 9))
			return false;
		m_field[x][y] = val;
		return true;
	}
	void clear()
	{
		for (auto& i : m_field)
			for (auto& j : i)
				j = 0;
	}

	bool ships_ready()
	{
		return (m_1 == 4 && m_2 == 3 && m_3 == 2 && m_4 == 1);
	}

	bool set_ship(int x, int y, int length, char direction)
	{

		auto check_bound = [](int x, int y)
		{
			return (x >= 0 && x <= 9 && y >= 0 && y <= 9);
		};

		if ((length < 1 || length > 4) ||
			(direction == 'v' && y + length - 1 > 9) ||
			(direction == 'h' && x + length - 1 > 9))
		{
			return false;
		} // length check

		for (int i = 0; i < length; ++i)
			if (!check_bound(x + i * (direction == 'h'), y + i * (direction == 'v')))
				return false; // bound check

		for (int i = -1; i < length + 1; ++i) 
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
				++m_1;
			break;
		case 3:
			if (m_3 == 2)
				return false;
			else
				++m_1;
			break;
		case 4:
			if (m_4 == 1)
				return false;
			else
				++m_1;
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
