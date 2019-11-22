#pragma once

#include <cstdlib>
#include <deque>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>

#include "message.hpp"

using boost::asio::ip::tcp;
typedef std::deque<message> message_queue;


class client
{
public:
	client(boost::asio::io_service& io_service, tcp::resolver::iterator endpoint_iterator);
	void write(const message& msg);
	void close();

private:

	void handle_connect(const boost::system::error_code& error);
	void handle_read_header(const boost::system::error_code& error);
	void handle_read_body(const boost::system::error_code& error);
	void do_write(message msg);
	void handle_write(const boost::system::error_code& error);
	void do_close();
	void gui_draw();

private:
	boost::asio::io_service& m_io_service_;
	tcp::socket m_socket;
	message m_read_msg;
	//message_queue m_write_msg_queue;
	std::deque<message> m_write_msg_queue;
	//client_gui_console m_client_gui;
	//set<int> m_participant_ids_table;
	//client_grid m_client_grid;
};
