#include "Server.h"

#include <iostream>
using namespace std;

bool Server::HandleMessage(const std::string& msg, participant* sender)
{
   switch (clentData[sender].state)
   {
   case ClientState::connected:
		return HandleMessageConnected(msg, sender);
   case ClientState::lobby:
		return HandleMessageLobby(msg, sender);
   case ClientState::session:
		return HandleMessageSession(msg, sender);
   }
}

bool Server::HandleMessageConnected(const std::string& msg, participant* sender)
{
	auto set_msg = [&](const char* line)
	{
		sendMsg.body_length(strlen(line));
		memcpy(sendMsg.body(), line, sendMsg.body_length());
		sendMsg.encode_header();
	};

	if (msg == "create") {
		Lobby& newLobby = lobbies[lobbiesCount];
		newLobby.id = lobbiesCount;
		lobbiesCount++;
		SharedClient& client = clentData[sender];
		lobby.AddParticipant(sender);
		client.state = ClientState::lobby;
		client.lobby = &newLobby;
		set_msg("c");
		WriteMsg(sendMsg, sender);
	}

	
	if (msg == "join") {
		bool check = false;
		for (int i = 0; i < lobbies.size(); i++) 
		{
			if (lobbies[i].AddParticipant(sender))
			{
				lobby = lobbies[i];
				lobby.id = i;
				lobby.AddParticipant(sender);
				SharedClient& client = clentData[sender];
				client.state = ClientState::lobby;
				client.lobby = &lobby;
				check = true;
			}
		}
		if (check)
		{
			check = false;
			set_msg("j");
		}
		else 
		{
			set_msg("r");
		}
		WriteMsg(sendMsg, sender);
	}

	if (std::string(msg.c_str(), 7) == "connect")
	{
		int id = atoi((msg.substr(11, msg.length()).c_str()));
		if (id <= lobbiesCount) 
		{
			if (lobbies[id].AddParticipant(sender))
			{
				lobby = lobbies[id];
				lobby.id = id;
				lobby.AddParticipant(sender);
				SharedClient& client = clentData[sender];
				client.state = ClientState::lobby;
				client.lobby = &lobby;
				
				set_msg("i");
				WriteMsg(sendMsg, sender);
			}
		}
		else
		{
			set_msg("r");
			WriteMsg(sendMsg, sender);
		}
	}

	if (msg == "list") {
		string id = "l ";
		for (auto i : lobbies) 
		{
			id = id + to_string(i.first) + ' ';
		}
		id = id + 'n';
		char array[100];
		set_msg(strcpy(array, id.c_str()));
		WriteMsg(sendMsg, sender);
	}

	if (msg == "quit") {
			cout << "Player leave" << endl;
			set_msg("q");
			WriteMsg(sendMsg, sender);
			return false;
	}
	return true;
}

bool Server::HandleMessageLobby(const std::string& msg, participant* sender)
{
	auto set_msg = [&](const char* line)
	{
		sendMsg.body_length(strlen(line));
		memcpy(sendMsg.body(), line, sendMsg.body_length());
		sendMsg.encode_header();
	};

	if (msg == "start") {
		SharedClient& client = clentData[sender];
		Lobby* newLobby = client.lobby;
		if (clentData[newLobby->GetEnemy(sender)].isReady)
		{
			clentData[sender].state = ClientState::session;
			clentData[newLobby->GetEnemy(sender)].state = ClientState::session;
			clentData[sender].isReady = true;
			set_msg("s");
			WriteMsg(sendMsg, sender);
			WriteMsg(sendMsg, (newLobby->GetEnemy(sender)));
		}
		else {
			set_msg("f");
			WriteMsg(sendMsg, sender);
		}
	}

	if (msg == "ready") {	
		SharedClient& client = clentData[sender];
		client.state = ClientState::lobby;
		clentData[sender].isReady = true;
		set_msg("r");
		WriteMsg(sendMsg, sender);
	}

	if (msg == "unready") {
		SharedClient& client = clentData[sender];
		client.state = ClientState::lobby;
		clentData[sender].isReady = false;
		set_msg("u");
		WriteMsg(sendMsg, sender);
	}

	if (msg == "leave") {
		cout << "Player leave" << endl;
		SharedClient& client = clentData[sender];
		client.state = ClientState::connected;
		client.lobby->RemoveParticipant(sender);
		if (!client.lobby->RemoveParticipant(sender)) 
		{
			lobbiesCount--;
			lobbies.erase(client.lobby->id);
		}
		set_msg("l");
		WriteMsg(sendMsg, sender);
	}

	if (msg == "quit") {
		cout << "Player leave" << endl;
		SharedClient& client = clentData[sender];
		client.lobby->RemoveParticipant(sender);
		if (!client.lobby->RemoveParticipant(sender))
		{
			lobbiesCount--;
			lobbies.erase(client.lobby->id);
		}
		set_msg("q");
		WriteMsg(sendMsg, sender);
		return false;
	}

	return true;
}

bool Server::HandleMessageSession(const std::string& msg, participant* sender)
{
	SharedClient& client = clentData[sender];
	SharedClient& enemy = clentData[client.lobby->GetEnemy(sender)];
	auto set_msg = [&](const char* line)
	{
		sendMsg.body_length(strlen(line));
		memcpy(sendMsg.body(), line, sendMsg.body_length());
		sendMsg.encode_header();
	};

	auto game_finished = [&](field* f)
	{
		for (int x = 0; x < 10; ++x)
			for (int y = 0; y < 10; ++y)
				if ((f->get_cell(x, y) & cell::ship) && !(f->get_cell(x, y) & cell::shot)) // есть неподстреленный корабль
					return false;
		return true;
	};

	auto swap_state = [&]()
	{
		if (client.lobby->IsParticipant1(sender))
			client.lobby->m_game.set_state(Game::egs::turn_2);
		else
			client.lobby->m_game.set_state(Game::egs::turn_1);
	};



	if (client.lobby->m_game.get_state() == Game::egs::preparation) {
		std::cout << "Preparation " << msg.size() << endl;
		if (msg.size() == 9)
			if (msg[0] == 's' && \
				msg[1] == ' ' && \
				msg[2] >= '0' && msg[2] <= '9' && \
				msg[3] == ' ' && \
				msg[4] >= '0' && msg[4] <= '9' && \
				msg[5] == ' ' && \
				msg[6] >= '1' && msg[6] <= '4' && \
				msg[7] == ' ' && \
				(msg[8] == 'v' || msg[8] == 'h')) {
				field* my_field;
				if (client.lobby->IsParticipant1(sender))
					my_field = &client.lobby->m_game.m_field_1;
				else
					my_field = &client.lobby->m_game.m_field_2;

				if (!my_field->set_ship(msg[2] - '0', msg[4] - '0', msg[6] - '0', msg[8])) {
					set_msg("f");
					cout << "Placement error" << endl;
					WriteMsg(sendMsg, sender);
					return true;
				}

				set_msg("d");
				WriteMsg(sendMsg, sender);
				return true;
			}
		if (msg.size() == 1)
			if (msg[0] == 'r') {
				if (client.lobby->IsParticipant1(sender))
					client.lobby->m_game.m_p1_ready = true;

				if (client.lobby->IsParticipant2(sender))
					client.lobby->m_game.m_p2_ready = true;

				if (client.lobby->m_game.m_p1_ready && client.lobby->m_game.m_p2_ready) {
					set_msg("t");
					WriteMsg(sendMsg, sender);
					set_msg("d");

					if (client.lobby->IsParticipant1(sender))
						client.lobby->m_game.set_state(Game::egs::turn_2);
					else
						client.lobby->m_game.set_state(Game::egs::turn_1);
					WriteLobby(sendMsg, sender);

					for (int y = 0; y < 10; ++y) {
						for (int x = 0; x < 10; ++x)
							cout << client.lobby->m_game.m_field_1.get_cell(x, y) << ' ';
						cout << endl;
					}
					cout << endl;

					for (int y = 0; y < 10; ++y) {
						for (int x = 0; x < 10; ++x)
							cout << client.lobby->m_game.m_field_2.get_cell(x, y) << ' ';
						cout << endl;
					}
					cout << endl;

					return true;
				}
				return true;
			}
	}
	if (client.lobby->m_game.get_state() == Game::egs::turn_1 || client.lobby->m_game.get_state() == Game::egs::turn_2) {
		cout << "Turns " << msg.size() << endl;
		if ((client.lobby->m_game.get_state() == Game::egs::turn_1 && client.lobby->IsParticipant1(sender)) || \
			(client.lobby->m_game.get_state() == Game::egs::turn_2 && client.lobby->IsParticipant2(sender))) {
			if (msg.size() == 3)
				if (msg[0] >= '0' && msg[0] <= '9' && \
					msg[1] == ' ' && \
					msg[2] >= '0' && msg[2] <= '9') {
					field* my_field;
					if (client.lobby->IsParticipant1(sender))
						my_field = &client.lobby->m_game.m_field_2;
					else
						my_field = &client.lobby->m_game.m_field_1;

					int x = msg[0] - '0';
					int y = msg[2] - '0';
					int c = my_field->get_cell(x, y);
					my_field->set_cell(x, y, c | cell::shot);


					if (c & cell::shot) {
						cout << "Turn repeat - failed " << msg << endl;
						set_msg("f"); // выстрел туда же?
						WriteMsg(sendMsg, sender);
						return true;
					}

					if (c & cell::ship) {
						cout << "Hit! " << msg << endl;
						char sh[] = "h x y";
						sh[2] = '0' + x;
						sh[4] = '0' + y;

						if (my_field->check_kill(x, y)) {
							my_field->kill(x, y);
							sh[0] = 'k';
						}

						set_msg(sh); // попадание

						if (game_finished(my_field)) {
							cout << "GG " << msg << endl;
							char sh[] = "g x y";
							sh[2] = '0' + x;
							sh[4] = '0' + y;
							set_msg(sh); // победа
							WriteLobby(sendMsg, sender);
							client.isWin = true;
							enemy.isWin = false;
							client.lobby->m_game.set_state(Game::egs::end);
							enemy.lobby->m_game.set_state(Game::egs::end);
							return true;
						}

						WriteLobby(sendMsg, sender);
						return true;
					} else {
						cout << "Miss " << msg << endl;
						char sh[] = "m x y";
						sh[2] = '0' + x;
						sh[4] = '0' + y;
						set_msg(sh); // попадание
						swap_state();
						WriteLobby(sendMsg, sender);
						return true;
					}
				}
		} else {
			cout << "Turn order - failed " << msg << endl;
			set_msg("f"); // ход не в своё время
			WriteMsg(sendMsg, sender);
			return true;
		}


		WriteLobby(sendMsg, sender);
		return true;
	}
	if (client.lobby->m_game.get_state() == Game::egs::end) {
		client.state = ClientState::lobby;
		enemy.state = ClientState::lobby;
		if (client.isWin)
		{
			set_msg("w"); // сейчас сообщения о том, кто победитель, никак не обрабатывается
			WriteMsg(sendMsg, sender);

			set_msg("l");
			WriteMsg(sendMsg, client.lobby->GetEnemy(sender));
		}
		else
		{
			set_msg("l"); // сейчас сообщения о том, кто победитель, никак не обрабатывается
			WriteMsg(sendMsg, sender);

			set_msg("w");
			WriteMsg(sendMsg, client.lobby->GetEnemy(sender));
		}
		return true;
	}
	set_msg("f"); // это сообщение об ошибке, да?
	WriteMsg(sendMsg, sender);
	cout << this << ": " << msg << endl;
}