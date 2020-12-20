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
		newLobby.AddParticipant(sender);
		client.state = ClientState::lobby;
		client.lobby = &newLobby;
		set_msg("c");
		WriteMsg(sendMsg, sender);
		return true;
	}

	
	if (msg == "join") {
		for (auto& lobbyPair : lobbies)
		{
			Lobby& lobby = lobbyPair.second;
		   if (lobby.AddParticipant(sender))
         {
            SharedClient& client = clentData[sender];
            client.state = ClientState::lobby;
            client.lobby = &lobby;
            set_msg("j");
            WriteMsg(sendMsg, sender);
				return true;
		   }
		}
      set_msg("r");
		WriteMsg(sendMsg, sender);
		return true;
	}

	if (std::string(msg.c_str(), 7) == "connect")
	{

		int id = atoi(msg.c_str() + 8);
		auto it = lobbies.find(id);
		if (it != lobbies.end()) 
		{
			Lobby& lobby = it->second;
			if (lobbies[id].AddParticipant(sender))
			{
				SharedClient& client = clentData[sender];
				client.state = ClientState::lobby;
				client.lobby = &lobby;
				
				set_msg("i");
				WriteMsg(sendMsg, sender);
				return true;
         } else {
            set_msg("r");
            WriteMsg(sendMsg, sender);
				return true;
         }
		}
		else
		{
			set_msg("r");
			WriteMsg(sendMsg, sender);
			return true;
		}
	}

	if (msg == "list") {
		string id = "l ";
		for (auto i : lobbies) 
		{
			id = id + to_string(i.first) + ' ';
		}
		id += " n";
		set_msg(id.c_str());
		WriteMsg(sendMsg, sender);
		return true;
	}

	if (msg == "quit") {
      std::cout << "Player leave" << endl;
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

	if (msg == "ready") {	
		SharedClient& client = clentData[sender];
      clentData[sender].isReady = true;
      set_msg("r");
      WriteMsg(sendMsg, sender);

      Lobby* newLobby = client.lobby;
      if (clentData[newLobby->GetEnemy(sender)].isReady)
      {
			client.lobby->m_game.m_field_1.clear();
			client.lobby->m_game.m_field_2.clear();
         client.state = ClientState::session;
         clentData[newLobby->GetEnemy(sender)].state = ClientState::session;
			client.lobby->m_game.m_game_state = Game::egs::preparation;
			set_msg("s");
			WriteLobby(sendMsg, sender);
      }

	}

	if (msg == "unready") {
		SharedClient& client = clentData[sender];
		client.state = ClientState::lobby;
		clentData[sender].isReady = false;
		set_msg("u");
		WriteMsg(sendMsg, sender);
	}

	if (msg == "leave") {
		std::cout << "Player leave" << endl;
		SharedClient& client = clentData[sender];
		client.state = ClientState::connected;
		client.lobby->RemoveParticipant(sender);
		if (!client.lobby->RemoveParticipant(sender)) 
		{
			lobbies.erase(client.lobby->id);
		}
		set_msg("l");
		WriteMsg(sendMsg, sender);
	}

	if (msg == "quit") {
		std::cout << "Player leave" << endl;
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
		if (msg.size() == 9 && 
			msg[0] == 's' && \
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
					std::cout << "Placement error" << endl;
					WriteMsg(sendMsg, sender);
					return true;
				}

				set_msg("d");
				WriteMsg(sendMsg, sender);
				return true;
			}
		if (msg == "ready") {
			if (client.lobby->IsParticipant1(sender))
				client.lobby->m_game.m_p1_ready = true;

			if (client.lobby->IsParticipant2(sender))
				client.lobby->m_game.m_p2_ready = true;

			if (client.lobby->m_game.m_p1_ready && client.lobby->m_game.m_p2_ready) {
				bool turn1 = rand() > 0;
				participant *first, *second;
				if (turn1)
				{
					first = client.lobby->m_participant_1;
               second = client.lobby->m_participant_2;
               client.lobby->m_game.set_state(Game::egs::turn_1);
				} else
				{
               first = client.lobby->m_participant_2;
               second = client.lobby->m_participant_1;
               client.lobby->m_game.set_state(Game::egs::turn_2);
				}

				set_msg("f");
				WriteMsg(sendMsg, first);
            set_msg("s");
            WriteMsg(sendMsg, second);

				for (int y = 0; y < 10; ++y) {
					for (int x = 0; x < 10; ++x)
						std::cout << client.lobby->m_game.m_field_1.get_cell(x, y) << ' ';
					std::cout << endl;
				}
				std::cout << endl;

				for (int y = 0; y < 10; ++y) {
					for (int x = 0; x < 10; ++x)
						std::cout << client.lobby->m_game.m_field_2.get_cell(x, y) << ' ';
					std::cout << endl;
				}
				std::cout << endl;

				return true;
			}
			return true;
		}
	}
	if (client.lobby->m_game.get_state() == Game::egs::turn_1 || client.lobby->m_game.get_state() == Game::egs::turn_2) {
		std::cout << "Turns " << msg.size() << endl;
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

					if (c & cell::shot) {
						std::cout << "Turn repeat - failed " << msg << endl;
						set_msg("f"); // выстрел туда же?
						WriteMsg(sendMsg, sender);
						return true;
					}

               my_field->set_cell(x, y, c | cell::shot);

					if (c & cell::ship) {
						std::cout << "Hit! " << msg << endl;
						char sh[] = "h x y";
						sh[2] = '0' + x;
						sh[4] = '0' + y;

						if (my_field->check_kill(x, y)) {
							my_field->kill(x, y);
							sh[0] = 'k';
						}
                                   
                  if (game_finished(my_field)) {
                     std::cout << "GG " << msg << endl;
                     sh[0] = 'g'; // победа
                     client.lobby->m_game.set_state(Game::egs::end);
                     enemy.lobby->m_game.set_state(Game::egs::end);
                     client.isReady = false;
                     enemy.isReady = false;
                     client.lobby->m_game.m_p1_ready = false;
                     client.lobby->m_game.m_p2_ready = false;
                  }

                  set_msg(sh); // попадание
                  WriteLobby(sendMsg, sender);
						return true;
					} else {
						std::cout << "Miss " << msg << endl;
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
			std::cout << "Turn order - failed " << msg << endl;
			set_msg("f"); // ход не в своё время
			WriteMsg(sendMsg, sender);
			return true;
		}


		WriteLobby(sendMsg, sender);
		return true;
	}
   if (client.lobby->m_game.get_state() == Game::egs::end) {
      if (msg == "continue") {
         set_msg("continue");
         WriteMsg(sendMsg, sender);
         client.state = ClientState::lobby;
         if (enemy.state == ClientState::lobby) {
            client.lobby->m_game.set_state(Game::egs::preparation);
         }
         client.isReady = false;
         return true;
      }
   }
	set_msg("f"); // это сообщение об ошибке, да?
	WriteMsg(sendMsg, sender);
	std::cout << this << ": " << msg << endl;
}