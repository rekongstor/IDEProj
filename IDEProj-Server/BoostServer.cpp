#include "BoostServer.h"

#include <boost/bind.hpp>

using boost::asio::ip::tcp;

BoostServer::BoostServer(boost::asio::io_service& io_service, const tcp::endpoint& endpoint)
	: m_io_service(io_service), m_acceptor(io_service, endpoint)
{
	m_room.myServer = this;
	start_accept();
}

void BoostServer::start_accept()
{
	session_ptr new_session(new BoostSession(m_io_service, m_room));
	m_acceptor.async_accept(new_session->socket(),
		boost::bind(&BoostServer::handle_accept, this, new_session,
			boost::asio::placeholders::error));
}

void BoostServer::handle_accept(session_ptr session,
	const boost::system::error_code& error)
{
	if (!error)
	{
		session->start();
	}

	start_accept();
}

void BoostServer::WriteMsg(const message& msg, participant* participant)
{
	std::string tmpMsg(msg.body(), msg.body_length());
	OutputDebugString("Server delivers message: ");
	OutputDebugString(tmpMsg.c_str());
	OutputDebugString("\n");

   participant->deliver(msg);
}

void BoostServer::WriteLobby(const message& msg, Lobby* lobby)
{
	std::string tmpMsg(msg.body(), msg.body_length());
	OutputDebugString("Server delivers message to lobby: ");
	OutputDebugString(tmpMsg.c_str());
	OutputDebugString("\n");

   lobby->m_participant_1->deliver(msg);
   lobby->m_participant_2->deliver(msg);
}
