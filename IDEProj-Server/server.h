#pragma once
#include "session.h"
#include <boost/asio.hpp>

typedef boost::shared_ptr<session> session_ptr;
/// <summary>
/// Класс сервера
/// </summary>
class server
{
public:
	/// <summary>
	/// Конструктор класса сервера
	/// </summary>
	/// <param name="io_service">Ссылка на io_service</param>
	/// <param name="endpoint">Ссылка на endpoint</param>
	server(boost::asio::io_service& io_service, const tcp::endpoint& endpoint);
	/// <summary>
	/// Метод начала приёма сообщений
	/// </summary>
	void start_accept();
	/// <summary>
	/// Асинхронный приём сообщений
	/// </summary>
	/// <param name="session">Указатель на сессию для приёма сообщения</param>
	/// <param name="error">Ссылка на ошибку</param>
	void handle_accept(session_ptr session, const boost::system::error_code& error);

private:
	boost::asio::io_service& m_io_service;
	tcp::acceptor m_acceptor;
	room m_room;
};
