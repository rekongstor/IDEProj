//
// chat_server.cpp
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2016 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <algorithm>
#include <cstdlib>
#include <deque>
#include <iostream>
#include <list>
#include <set>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
#include "message.hpp"

using boost::asio::ip::tcp;

//----------------------------------------------------------------------

typedef std::deque<message> message_queue;

//----------------------------------------------------------------------

class participant
{
public:
	virtual ~participant() {}
	virtual void deliver(const message& msg) = 0;
};

typedef boost::shared_ptr<participant> participant_ptr;

//----------------------------------------------------------------------

class room
{
public:
	void join(participant_ptr participant)
	{
		m_participants.insert(participant);
		//std::for_each(recent_msgs_.begin(), recent_msgs_.end(),
		//	boost::bind(&participant::deliver, participant, _1));
	}

	void leave(participant_ptr participant)
	{
		m_participants.erase(participant);
	}

	void deliver(const message& msg)
	{
		//recent_msgs_.push_back(msg);
		//while (recent_msgs_.size() > max_recent_msgs)
		//	recent_msgs_.pop_front();

		std::for_each(m_participants.begin(), m_participants.end(),
			boost::bind(&participant::deliver, _1, boost::ref(msg)));
	}

private:
	std::set<participant_ptr> m_participants;
	//enum { max_recent_msgs = 100 };
	//message_queue recent_msgs_;
};

//----------------------------------------------------------------------

class session
	: public participant,
	public boost::enable_shared_from_this<session>
{
public:
	session(boost::asio::io_service& io_service, room& room)
		: m_socket(io_service),
		m_room(room)
	{
	}

	tcp::socket& socket()
	{
		return m_socket;
	}

	void start()
	{
		m_room.join(shared_from_this());
		boost::asio::async_read(m_socket,
			boost::asio::buffer(m_read_msg.data(), message::header_length),
			boost::bind(
				&session::handle_read_header, shared_from_this(),
				boost::asio::placeholders::error));
	}

	void deliver(const message& msg)
	{
		bool write_in_progress = !m_write_msgs_queue.empty();
		m_write_msgs_queue.push_back(msg);
		if (!write_in_progress)
		{
			boost::asio::async_write(m_socket,
				boost::asio::buffer(m_write_msgs_queue.front().data(),
					m_write_msgs_queue.front().length()),
				boost::bind(&session::handle_write, shared_from_this(),
					boost::asio::placeholders::error));
		}
	}

	void handle_read_header(const boost::system::error_code& error)
	{
		if (!error && m_read_msg.decode_header())
		{
			boost::asio::async_read(m_socket,
				boost::asio::buffer(m_read_msg.body(), m_read_msg.body_length()),
				boost::bind(&session::handle_read_body, shared_from_this(),
					boost::asio::placeholders::error));
		}
		else
		{
			m_room.leave(shared_from_this());
		}
	}

	void handle_read_body(const boost::system::error_code& error)
	{
		if (!error)
		{
			m_room.deliver(m_read_msg);
			boost::asio::async_read(m_socket,
				boost::asio::buffer(m_read_msg.data(), message::header_length),
				boost::bind(&session::handle_read_header, shared_from_this(),
					boost::asio::placeholders::error));
		}
		else
		{
			m_room.leave(shared_from_this());
		}
	}

	void handle_write(const boost::system::error_code& error)
	{
		if (!error)
		{
			m_write_msgs_queue.pop_front();
			if (!m_write_msgs_queue.empty())
			{
				boost::asio::async_write(m_socket,
					boost::asio::buffer(m_write_msgs_queue.front().data(),
						m_write_msgs_queue.front().length()),
					boost::bind(&session::handle_write, shared_from_this(),
						boost::asio::placeholders::error));
			}
		}
		else
		{
			m_room.leave(shared_from_this());
		}
	}

private:
	tcp::socket m_socket;
	room& m_room;
	message m_read_msg;
	message_queue m_write_msgs_queue;
};

typedef boost::shared_ptr<session> session_ptr;

//----------------------------------------------------------------------

class server
{
public:
	server(boost::asio::io_service& io_service,
		const tcp::endpoint& endpoint)
		: m_io_service(io_service),
		m_acceptor(io_service, endpoint)
	{
		start_accept();
	}

	void start_accept()
	{
		session_ptr new_session(new session(m_io_service, m_room));
		m_acceptor.async_accept(new_session->socket(),
			boost::bind(&server::handle_accept, this, new_session,
				boost::asio::placeholders::error));
	}

	void handle_accept(session_ptr session,
		const boost::system::error_code& error)
	{
		if (!error)
		{
			session->start();
		}

		start_accept();
	}

private:
	boost::asio::io_service& m_io_service;
	tcp::acceptor m_acceptor;
	room m_room;
};

typedef boost::shared_ptr<server> server_ptr;
typedef std::list<server_ptr> chat_server_list;

//----------------------------------------------------------------------

int main(int argc, char* argv[])
{
	try
	{
		if (argc < 2)
		{
			std::cerr << "Usage: chat_server <port> [<port> ...]\n";
			return 1;
		}

		boost::asio::io_service io_service;

		chat_server_list servers;
		for (int i = 1; i < argc; ++i)
		{
			using namespace std; // For atoi.
			tcp::endpoint endpoint(tcp::v4(), atoi(argv[i]));
			server_ptr server(new server(io_service, endpoint));
			servers.push_back(server);
		}

		io_service.run();
	}
	catch (std::exception & e)
	{
		std::cerr << "Exception: " << e.what() << "\n";
	}

	return 0;
}