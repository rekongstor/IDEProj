#pragma once

#include "participant.h"
#include <boost/shared_ptr.hpp>
#include <map>
#include <set>

#include "Server.h"


typedef boost::shared_ptr<participant> participant_ptr;
/// <summary>
/// Класс комнаты
/// </summary>
class BoostRoom
{
public:
	/// <summary>
	/// Добавить игрока в комнату
	/// </summary>
	/// <param name="participant">Указатель на игрока</param>
	void join(participant_ptr participant);
	/// <summary>
	/// Удалить игрока из комнаты
	/// </summary>
	/// <param name="participant">Указатель на игрока</param>
	void leave(participant_ptr participant);
	/// <summary>
	/// Отправить сообщение всем игрокам в комнате
	/// </summary>
	/// <param name="msg">Ссылка на сообщение</param>
	void deliver(const message& msg);
	/// <summary>
	/// Отправить сообщение конкретному игроку
	/// </summary>
	/// <param name="msg">Ссылка на сообщение</param>
	void deliver(const message& msg, void* participantPtr);

	Server* myServer;
private:
	/// <summary>
	/// Set из игроков
	/// </summary>
	std::set<participant_ptr> m_participants;
	/// <summary>
	/// Объект game, состояния игры
	/// </summary>
};
