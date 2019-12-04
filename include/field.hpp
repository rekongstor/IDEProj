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
	bool set_cell(int, int, int);
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
	if ((x < 0) || (x > 9) || (y < 0) || (y > 9))
		return m_field[0][0];
	return m_field[x][y];
}
bool field::set_cell(int x, int y, int val)
{
	if ((x < 0) || (x > 9) || (y < 0) || (y > 9))
		return false;
	m_field[x][y] = val;
	return true;
}
void field::clear()
{
	for (auto& i : m_field)
		for (auto& j : i)
			j = 0;
}