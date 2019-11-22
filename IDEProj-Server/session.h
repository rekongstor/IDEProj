#pragma once
#include "participant.h"
#include "room.h"
#include <deque>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>

typedef std::deque<message> message_queue;

using boost::asio::ip::tcp;

class session : public participant,	public boost::enable_shared_from_this<session>
{
public:
	session(boost::asio::io_service& io_service, room& room);
	tcp::socket& socket();
	void start();
	void deliver(const message& msg);
	void handle_read_header(const boost::system::error_code& error);
	void handle_read_body(const boost::system::error_code& error);
	void handle_write(const boost::system::error_code& error);

private:
	tcp::socket m_socket;
	room& m_room;
	message m_read_msg;
	message_queue m_write_msgs_queue;
};


