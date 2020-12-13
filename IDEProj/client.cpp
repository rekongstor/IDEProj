#include "Client.h"

#include <iostream>

#include "ConsoleGui.h"

using namespace std;

void Client::HandleMessage(const std::string& msg)
{
   switch (state)
   {
   case ClientState::connected:
      break;
   case ClientState::lobby:
      break;
   case ClientState::session:
      break;
   }
}

void Client::HandleMessageConnected(const std::string& msg)
{
}

void Client::HandleMessageLobby(const std::string& msg)
{
}

void Client::HandleMessageSession(const std::string& msg)
{
	switch (gameState) {
	case egs::preparation:
	{
		if (ready == true) {
			switch (msg[0]) {
			case 't':
				gui->draw();
				second_turn = true;
				receive = false;
				break;

			case 'd':
				if (second_turn)
					gameState = egs::enemy_turn;
				else
					gameState = egs::my_turn;

				gui->draw();
				receive = false;
				break;
			}
		} else {
			switch (msg[0]) {
			case 'd':
				gui->draw();
				receive = false;
				break;
			case 'f':
				gui->draw();
				receive = false;
				cout << "Server error happened. This is really bad." << endl;
				break;
			}
		}
		break;
	}

	case egs::my_turn:
	{
		switch (msg[0]) {
		case 'h':

			m_en.set_cell(msg[2] - '0', msg[4] - '0', cell::ship | cell::shot);
			gui->draw();
			receive = false; // продолжаем стрелять
			break;

		case 'k':
			m_en.set_cell(msg[2] - '0', msg[4] - '0', cell::ship | cell::shot);
			m_en.kill(msg[2] - '0', msg[4] - '0');
			gui->draw();
			receive = false; // продолжаем стрелять
			break;

		case 'm':
			m_en.set_cell(msg[2] - '0', msg[4] - '0', cell::shot);
			gameState = egs::enemy_turn;
			gui->draw();
			receive = false; // передаём ход сопернику
			break;

		case 'g':
			m_en.set_cell(msg[2] - '0', msg[4] - '0', cell::ship | cell::shot);
			m_en.kill(msg[2] - '0', msg[4] - '0');
			gameState = egs::end;
			gui->draw();
			cout << "\x1B[92mYou won!\x1B[0m" << endl;
			receive = false; // продолжаем стрелять
			break;

		case 'f':
			gui->draw();
			cout << "\x1B[93m";
			cout << "Turn repeat" << endl;
			cout << "\x1B[0m";
			receive = false; // продолжаем стрелять
			break;
		default:
			throw(std::exception("Wrong message received?"));

		}
		break;
	}

	case egs::enemy_turn:
	{
		switch (msg[0]) {
		case 'h':
			m_my.set_cell(msg[2] - '0', msg[4] - '0', cell::ship | cell::shot);
			gui->draw();
			receive = false; // продолжаем стрелять
		case 'k':
			m_my.set_cell(msg[2] - '0', msg[4] - '0', cell::ship | cell::shot);
			m_my.kill(msg[2] - '0', msg[4] - '0');
			gui->draw();
			receive = false; // продолжаем стрелять
		case 'm':
			m_my.set_cell(msg[2] - '0', msg[4] - '0', cell::shot);
			gameState = egs::my_turn;
			gui->draw();
			receive = false; // передаём ход сопернику
		case 'f':
			gui->draw();
			cout << "Not your turn";
			receive = false; // продолжаем стрелять
		case 'g':
			m_my.set_cell(msg[2] - '0', msg[4] - '0', cell::ship | cell::shot);
			m_my.kill(msg[2] - '0', msg[4] - '0');
			gameState = egs::end;
			gui->draw();
			cout << "\x1B[91mYou lost!\x1B[0m" << endl;
			receive = false;
		default:
			throw(std::exception("Wrong message received?"));
		}
		break;
	}

	case egs::end:
	{
		gui->draw();
		//std::cout << "\x1B[93m";
		//std::cout.write(m_read_msg.body(), m_read_msg.body_length());
		//std::cout << "\n";
		//std::cout << "\x1B[0m";
		break;
	}
	}
}
