#include "Client.h"

#include <iostream>
#include <vector>
#include <sstream>

#include "ConsoleGui.h"
#include "message.hpp"

using namespace std;

void Client::HandleMessage(const std::string& msg)
{
   switch (state)
   {
   case ClientState::connected:
		HandleMessageConnected(msg);
      break;
   case ClientState::lobby:
		HandleMessageLobby(msg);
      break;
   case ClientState::session:
		HandleMessageSession(msg);
      break;
   }
}

void Client::HandleSendMessage(const std::string& line)
{
		using namespace std; // For strlen and memcpy.
		message msg;
		msg.body_length(line.size());
		memcpy(msg.body(), line.c_str(), msg.body_length());
		msg.encode_header();

		switch (state)
		{
		case ClientState::connected:
			HandleSendMessageConnected(line, msg);
			break;

		case ClientState::lobby:
			HandleSendMessageLobby(line, msg);
			break;

      case ClientState::session:
			HandleSendMessageSession(line, msg);
			break;
		}
}

void Client::HandleMessageConnected(const std::string& msg)
{
	const char* message = msg.c_str();
	int i = 2;
	switch (msg[0])
	{
	case 'q':
		cout << "Thanks for playing!" << endl;
		break;
	
	case 'j':
		cout << "You joined lobby. Moving directly to the session for now" << endl;
		state = ClientState::lobby;
		receive = false;
		break;
	
	case 'c':
		cout << "Lobby was created. Moving directly to the session for now" << endl;
		state = ClientState::lobby;
		receive = false;
		break;

	case 'i':
		cout << "You joined lobby. Moving directly to the session for now" << endl;
		state = ClientState::lobby;
		receive = false;
		break;

	case 'r':
		cout << "Okay, we don't have this lobby :(" << endl;
		state = ClientState::connected;
		receive = false;
		break;

   case 'l':
		extern void UpdateList(const char* lList);
		UpdateList(message + 2);

		break;

	default:
		throw(std::exception("Wrong message received?"));
		receive = false;
		break;
	}
	gui->draw();
}

void Client::HandleSendMessageConnected(const std::string& line, message& msg)
{
	if (line == "join" || line == "create" || line == "quit" || line == "list" || std::string(line.c_str(), 7) == "connect")
	{
		write(msg);
		receive = false;
	} 
	else
	{
		cout << "Invalid input!" << endl;
		receive = false;
		return;
	}
}

void Client::HandleMessageLobby(const std::string& msg)
{
	switch (msg[0])
	{
	case 'l':
		cout << "Thanks for playing! You will be returned to home." << endl;
		receive = false;
		state = ClientState::connected;
		break;

	case 'q':
		cout << "Thanks for playing! Bye!" << endl;
		break;

	case 'u':
		cout << "Now you are unready." << endl;
		state = ClientState::lobby;
		receive = false;
		break;

	case 'r':
		cout << "You are ready to start the game. The game is starting soon!" << endl;
		state = ClientState::lobby;
		receive = true;
		break;

	case 's':
		cout << "Let's start the game. Good luck!" << endl;
		state = ClientState::session;
		receive = false;
		break;

	case 'f':
		cout << "Waiting for all player..." << endl;;
		receive = false;
		break;

	default:
		throw(std::exception("Wrong message received?"));
		receive = false;
		break;
	}
	gui->draw();
}

void Client::HandleSendMessageLobby(const std::string& line, message& msg)
{
	if (line == "ready" || line == "start" || line == "quit" || line == "unready" || line == "quit" || line == "leave")
	{
		write(msg);
		receive = false;
	}
	else
	{
		cout << "Invalid input!" << endl;
		return;
	}
}

void Client::HandleMessageSession(const std::string& msg)
{
	switch (gameState) {
	case egs::preparation:
	{
		if (ready == true) {
			switch (msg[0]) 
			{
			case 'f':
				second_turn = false;
				receive = false;
				break;

         case 's':
            second_turn = true;

				receive = false;
				break;
         }
         if (second_turn)
            gameState = egs::enemy_turn;
         else
            gameState = egs::my_turn;
         gui->draw();
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
		switch (msg[0]) 
		{
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
		switch (msg[0]) 
		{
		case 'h':
			m_my.set_cell(msg[2] - '0', msg[4] - '0', cell::ship | cell::shot);
			gui->draw();
			receive = false; // продолжаем стрелять
			break;

		case 'k':
			m_my.set_cell(msg[2] - '0', msg[4] - '0', cell::ship | cell::shot);
			m_my.kill(msg[2] - '0', msg[4] - '0');
			gui->draw();
			receive = false; // продолжаем стрелять
			break;

		case 'm':
			m_my.set_cell(msg[2] - '0', msg[4] - '0', cell::shot);
			gameState = egs::my_turn;
			gui->draw();
			receive = false; // передаём ход сопернику
			break;

		case 'f':
			gui->draw();
			cout << "Not your turn";
			receive = false; // продолжаем стрелять
			break;

		case 'g':
			m_my.set_cell(msg[2] - '0', msg[4] - '0', cell::ship | cell::shot);
			m_my.kill(msg[2] - '0', msg[4] - '0');
			gameState = egs::end;
			gui->draw();
			cout << "\x1B[91mYou lost!\x1B[0m" << endl;
			receive = false;
			break;

		default:
			throw(std::exception("Wrong message received?"));
		}
		break;
	}

	case egs::end:
	{
		cout << "You should proceed to the lobby." << endl;
		gui->draw();
		//std::cout << "\x1B[93m";
		//std::cout.write(m_read_msg.body(), m_read_msg.body_length());
		//std::cout << "\n";
		//std::cout << "\x1B[0m";
		break;
	}
	}
}

void Client::HandleSendMessageSession(const std::string& line, message& msg)
{
	if (gameState == egs::preparation) {
		if (line.size() == 9 && line[0] == 's' && line[2] >= '0' &&
			line[2] <= '9' && line[4] >= '0' && line[4] <= '9' &&
			line[6] >= '1' && line[6] <= '4' && (line[8] == 'v' || line[8] == 'h')) {
			if (!m_my.set_ship(line[2] - '0', line[4] - '0', line[6] - '0', line[8])) {
				gui->draw();
				cout << "\x1B[31mInvalid placement!\x1B[0m" << endl;
				return;
			}
			// we want to check here our placement
			write(msg);
			receive = true;
			return;
		} // s x y w d command

		if (line == "ready") {
			if (m_my.ships_ready()) {
				ready = true;
				write(msg);
				receive = true;
				return;
			}
			else {
				gui->draw();
				cout << "\x1B[31mShips not ready!\x1B[0m" << endl;
				return;
			}
		}
		gui->draw();
		cout << "\x1B[31mInvalid input!\x1B[0m" << endl;
		// otherwise we don't send anything
	}
	// turns
	if (gameState == egs::my_turn) {
		if (line.size() == 3 && line[0] >= '0' && line[0] <= '9' &&
			line[2] >= '0' && line[2] <= '9') {
			write(msg);
			receive = true;
			return;
		}
		// otherwise we don't send anything
	}

	if (gameState == egs::end) {
		write(msg);
	}
}
