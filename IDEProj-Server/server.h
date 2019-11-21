#pragma once
#include "session.h"
#include <boost/asio.hpp>

typedef boost::shared_ptr<session> session_ptr;

class server
{
public:
	server(boost::asio::io_service& io_service, const tcp::endpoint& endpoint);
	void start_accept();
	void handle_accept(session_ptr session, const boost::system::error_code& error);

private:
	boost::asio::io_service& m_io_service;
	tcp::acceptor m_acceptor;
	room m_room;
};
