#include "Game.h"

Game::Game() : m_game_state(egs::preparation)
{
}

void Game::set_state(egs gs)
{
	m_game_state = gs;
}

Game::egs Game::get_state()
{
	return m_game_state;
}
