#pragma once

enum cell
{
	ship = 1 << 0,
	shot = 1 << 1
};

class field
{
public:
	field();
	const int& get_cell(int, int);
	void set_cell(int, int, int);
	void clear();

private:
	int m_field[10][10];
};

field::field()
{
	clear();
}
const int& field::get_cell(int x, int y)
{
	return m_field[x][y];
}
void field::set_cell(int x, int y, int val)
{
	m_field[x][y] = val;
}
void field::clear()
{
	for (auto& i : m_field)
		for (auto& j : i)
			j = 0;
}