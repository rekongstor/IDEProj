#pragma once
#include "participant.h"
#include "room.h"
#include <deque>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>

typedef std::deque<message> message_queue;

using boost::asio::ip::tcp;
/// <summary>
/// Класс сервер-сессии клиента
/// </summary>
class session : public participant,	public boost::enable_shared_from_this<session>
{
public:
	/// <summary>
	/// Конструктор по умолчанию
	/// </summary>
	/// <param name="io_service">Ссылка на io_service</param>
	/// <param name="room">Ссылка на подключаемую комнату</param>
	session(boost::asio::io_service& io_service, room& room);
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
	void deliver(const message& msg);
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

private:
	/// <summary>
	/// Сокет
	/// </summary>
	tcp::socket m_socket;
	/// <summary>
	/// Ссылка на комнату сессии
	/// </summary>
	room& m_room;
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


