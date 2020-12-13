#pragma once
#include "BoostSession.h"
#include <boost/asio.hpp>

typedef boost::shared_ptr<BoostSession> session_ptr;
/// <summary>
/// Класс сервера
/// </summary>
class BoostServer : public Server
{
public:
	/// <summary>
	/// Конструктор класса сервера
	/// </summary>
	/// <param name="io_service">Ссылка на io_service</param>
	/// <param name="endpoint">Ссылка на endpoint</param>
	BoostServer(boost::asio::io_service& io_service, const tcp::endpoint& endpoint);
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
	
	void WriteMsg(const message& msg, participant* participant) override;
	void WriteLobby(const message& msg, Lobby* lobby) override;
private:
	boost::asio::io_service& m_io_service;
	tcp::acceptor m_acceptor;
	BoostRoom m_room;
};
