#pragma once

// Возможные состояния клиента
enum class ClientState
{
	connected,
	lobby,
	session
};

class Lobby;

struct SharedClient
{
	ClientState state = ClientState::connected;
	Lobby* lobby;
};
