#pragma once
#include <map>
#include <set>
#include <string>
#include "message.hpp"
#include "Lobby.h"
#include "participant.h"
#include "shared.hpp"

class message;

class Server
{
public:

	int lobbiesCount = 0;
	std::map<int, Lobby> lobbies;
	std::map<participant*, SharedClient> clentData;
	message sendMsg;

	Lobby lobby;

   bool HandleMessage(const std::string& msg, participant* sender);
   bool HandleMessageConnected(const std::string& msg, participant* sender);
   bool HandleMessageLobby(const std::string& msg, participant* sender);
   bool HandleMessageSession(const std::string& msg, participant* sender);

	virtual void WriteMsg(const message& msg, participant* participant) = 0;
	virtual void WriteLobby(const message& msg, participant* participant) = 0;
	virtual void WriteEnemy(const message& msg, participant* participant) = 0;
};

