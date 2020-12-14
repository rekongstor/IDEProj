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
	std::map<int, Lobby> lobbies;
	std::map<participant*, SharedClient> clentData;
	message sendMsg;

	Lobby TEMPORARY;

	virtual void WriteMsg(const message& msg, participant* participant) = 0;
	virtual void WriteLobby(const message& msg, participant* participant) = 0;
	virtual void WriteEnemy(const message& msg, participant* participant) = 0;
   bool HandleMessage(const std::string& msg, participant* sender);
   bool HandleMessageConnected(const std::string& msg, participant* sender);
   bool HandleMessageLobby(const std::string& msg, participant* sender);
   bool HandleMessageSession(const std::string& msg, participant* sender);
};

