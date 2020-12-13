#pragma once
#include <string>

#include "ConsoleGui.h"
#include "field.hpp"
#include "shared.hpp"

class message;

enum class egs
{
	preparation,
	my_turn,
	enemy_turn,
	end
};

class Client
{
public:  
	egs gameState;
	ClientState state;
	ConsoleGui* gui;
	
	/// Переменная, отвечающая за готовность расстановки кораблей
	bool ready = false;
	/// Переменная, отвечающая за порядок ходов: true - игрок ходит вторым, false - игрок ходит первым
	bool second_turn = false;
	/// Игровое поле игрока
	field m_my;
	/// Игрокове поле соперника
	field m_en;
	/// Ожидание ответа от сервера
	bool receive;

	void HandleMessage(const std::string& msg);
	void HandleMessageConnected(const std::string& msg);
	void HandleMessageLobby(const std::string& msg);
	void HandleMessageSession(const std::string& msg);

	void HandleSendMessage(const std::string& line);

	virtual void write(const message& msg) = 0;
};

