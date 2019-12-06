#include "server.h"

#include <boost/bind.hpp>

using boost::asio::ip::tcp;

server::server(boost::asio::io_service& io_service, const tcp::endpoint& endpoint)
	: m_io_service(io_service), m_acceptor(io_service, endpoint)
{
	start_accept();
}

void server::start_accept()
{
	session_ptr new_session(new session(m_io_service, m_room));
	m_acceptor.async_accept(new_session->socket(),
		boost::bind(&server::handle_accept, this, new_session,
			boost::asio::placeholders::error));
}

void server::handle_accept(session_ptr session,
	const boost::system::error_code& error)
{
	if (!error)
	{
		session->start();
	}

	start_accept();
}