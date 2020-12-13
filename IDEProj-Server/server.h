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
	std::set<Lobby> lobbies;
	std::map<participant*, SharedClient> clentData;
   message sendMsg;

	Lobby TEMPORARY;

	virtual void WriteMsg(const message& msg, participant* participant) = 0;
	virtual void WriteLobby(const message& msg, Lobby* lobby) = 0;
	void HandleMessage(const std::string& msg, participant* sender);
	void HandleMessageConnected(const std::string& msg, participant* sender);
	void HandleMessageLobby(const std::string& msg, participant* sender);
	void HandleMessageSession(const std::string& msg, participant* sender);
};

