#pragma once

// Возможные состояния клиента
enum class ClientState
{
	connected,
	lobby,
	session
};

struct SharedClient
{
	ClientState state;
};
