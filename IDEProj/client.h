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
	ClientState state = ClientState::connected;
	ConsoleGui* gui;
	
	/// ����������, ���������� �� ���������� ����������� ��������
	bool ready = false;
	/// ����������, ���������� �� ������� �����: true - ����� ����� ������, false - ����� ����� ������
	bool second_turn = false;
	/// ������� ���� ������
	field m_my;
	/// �������� ���� ���������
	field m_en;
	/// �������� ������ �� �������
	bool receive;

	void HandleMessage(const std::string& msg);
	void HandleMessageConnected(const std::string& msg);
	void HandleMessageLobby(const std::string& msg);
	void HandleMessageSession(const std::string& msg);

   void HandleSendMessage(const std::string& line);
   void HandleSendMessageConnected(const std::string& line, message& msg);
   void HandleSendMessageLobby(const std::string& line, message& msg);
   void HandleSendMessageSession(const std::string& line, message& msg);

	virtual void write(const message& msg) = 0;
};

