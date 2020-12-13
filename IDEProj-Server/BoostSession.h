#pragma once
#include "participant.h"
#include "BoostRoom.h"
#include <deque>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>

#include "Server.h"

typedef std::deque<message> message_queue;

using boost::asio::ip::tcp;
/// <summary>
/// Класс сервер-сессии клиента
/// </summary>
class BoostSession : public participant,	public boost::enable_shared_from_this<BoostSession>
{
public:
	/// <summary>
	/// Конструктор по умолчанию
	/// </summary>
	/// <param name="io_service">Ссылка на io_service</param>
	/// <param name="room">Ссылка на подключаемую комнату</param>
	BoostSession(boost::asio::io_service& io_service, BoostRoom& room);
	/// <summary>
	/// Функция, возвращающая текущий сокет
	/// </summary>
	/// <returns></returns>
	tcp::socket& socket();
	/// <summary>
	/// Метод запуска сессии
	/// </summary>
	void start();
	/// <summary>
	/// Метод отправки сообщений клиенту
	/// </summary>
	/// <param name="msg"></param>
	void deliver(const message& msg) override;
	/// <summary>
	/// Асинхронное чтение заголовка сообщения
	/// </summary>
	/// <param name="error">Ссылка на ошибку</param>
	void handle_read_header(const boost::system::error_code& error);
	/// <summary>
	/// Асинхронное чтение теста сообщения
	/// </summary>
	/// <param name="error">Ссылка на ошибку</param>
	void handle_read_body(const boost::system::error_code& error);
	/// <summary>
	/// Асинхронная запись сообщения
	/// </summary>
	/// <param name="error"></param>
	void handle_write(const boost::system::error_code& error);

	// Delivers message to participant
	void WriteMsg(const message& msg);
	// Delivers message to all participants in the lobby
	void WriteLobby(const message& msg, Lobby* lobby);
private:
	/// <summary>
	/// Сокет
	/// </summary>
	tcp::socket m_socket;
	/// <summary>
	/// Ссылка на комнату сессии
	/// </summary>
	BoostRoom& m_room;
	/// <summary>
	/// Сообщение сессии
	/// </summary>
	message m_read_msg;
	/// <summary>
	/// Очередь сообщений сессии
	/// </summary>
	message_queue m_write_msgs_queue;

	void decode_body();
};


