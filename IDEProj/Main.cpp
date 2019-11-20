//
// chat_client.cpp
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2016 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

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
	client(boost::asio::io_service& io_service,
		tcp::resolver::iterator endpoint_iterator)
		: m_io_service_(io_service),
		m_socket(io_service)
	{
		boost::asio::async_connect(m_socket, endpoint_iterator,
			boost::bind(&client::handle_connect, this,
				boost::asio::placeholders::error));
	}

	void write(const message& msg)
	{
		m_io_service_.post(boost::bind(&client::do_write, this, msg));
	}

	void close()
	{
		m_io_service_.post(boost::bind(&client::do_close, this));
	}

private:

	void handle_connect(const boost::system::error_code& error)
	{
		if (!error)
		{
			boost::asio::async_read(m_socket,
				boost::asio::buffer(m_read_msg.data(), message::header_length),
				boost::bind(&client::handle_read_header, this,
					boost::asio::placeholders::error));
		}
	}

	void handle_read_header(const boost::system::error_code& error)
	{
		if (!error && m_read_msg.decode_header())
		{
			boost::asio::async_read(m_socket,
				boost::asio::buffer(m_read_msg.body(), m_read_msg.body_length()),
				boost::bind(&client::handle_read_body, this,
					boost::asio::placeholders::error));
		}
		else
		{
			do_close();
		}
	}

	void handle_read_body(const boost::system::error_code& error)
	{
		if (!error)
		{
			std::cout.write(m_read_msg.body(), m_read_msg.body_length());
			std::cout << "\n";
			boost::asio::async_read(m_socket,
				boost::asio::buffer(m_read_msg.data(), message::header_length),
				boost::bind(&client::handle_read_header, this,
					boost::asio::placeholders::error));
		}
		else
		{
			do_close();
		}
	}

	void do_write(message msg)
	{
		bool write_in_progress = !m_write_msg_queue.empty();
		m_write_msg_queue.push_back(msg);
		if (!write_in_progress)
		{
			boost::asio::async_write(m_socket,
				boost::asio::buffer(m_write_msg_queue.front().data(),
					m_write_msg_queue.front().length()),
				boost::bind(&client::handle_write, this,
					boost::asio::placeholders::error));
		}
	}

	void handle_write(const boost::system::error_code& error)
	{
		if (!error)
		{
			m_write_msg_queue.pop_front();
			if (!m_write_msg_queue.empty())
			{
				boost::asio::async_write(m_socket,
					boost::asio::buffer(m_write_msg_queue.front().data(),
						m_write_msg_queue.front().length()),
					boost::bind(&client::handle_write, this,
						boost::asio::placeholders::error));
			}
		}
		else
		{
			do_close();
		}
	}

	void do_close()
	{
		m_socket.close();
	}

	void gui_draw() {

	}

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

int main(int argc, char* argv[])
{
	try
	{
		if (argc != 3)
		{
			std::cerr << "Usage: chat_client <host> <port>\n";
			return 1;
		}

		boost::asio::io_service io_service;

		tcp::resolver resolver(io_service);
		tcp::resolver::query query(argv[1], argv[2]);
		tcp::resolver::iterator iterator = resolver.resolve(query);

		client c(io_service, iterator);

		boost::thread t(boost::bind(&boost::asio::io_service::run, &io_service));

		char line[message::max_body_length + 1];
		while (std::cin.getline(line, message::max_body_length + 1))
		{
			using namespace std; // For strlen and memcpy.
			message msg;
			msg.body_length(strlen(line));
			memcpy(msg.body(), line, msg.body_length());
			msg.encode_header();
			c.write(msg);
		}

		c.close();
		t.join();
	}
	catch (std::exception & e)
	{
		std::cerr << "Exception: " << e.what() << "\n";
	}

	return 0;
}