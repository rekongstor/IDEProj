#pragma once

enum cell
{
	ship = 1 << 0,
	shot = 1 << 1
};

class field
{
public:
	field()
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

	bool set_ship(int x, int y, int length, char direction)
	{
		for (int i = 0; i < length; ++i)
		{
			set_cell(x + i * (direction == 'h'), y + i * (direction == 'v'), cell::ship);
		}
	}

private:
	int m_field[10][10];
};
