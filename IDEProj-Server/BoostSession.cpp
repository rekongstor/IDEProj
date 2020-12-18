#include "BoostSession.h"

#include <boost/bind.hpp>
#include <iostream>
using namespace std;

BoostSession::BoostSession(boost::asio::io_service& io_service, BoostRoom& room)
	: m_socket(io_service),
	m_room(room)
{
}

tcp::socket& BoostSession::socket()
{
	return m_socket;
}

void BoostSession::start()
{
	m_room.join(shared_from_this());

	boost::asio::async_read(m_socket,
		boost::asio::buffer(m_read_msg.data(), message::header_length),
		boost::bind(
			&BoostSession::handle_read_header, shared_from_this(),
			boost::asio::placeholders::error));
}

void BoostSession::deliver(const message& msg)
{
   std::string dbgMsg(msg.body(), msg.body_length());
   OutputDebugString("Server delivers message: ");
   OutputDebugString(dbgMsg.c_str());
   OutputDebugString("\n");

	bool write_in_progress = !m_write_msgs_queue.empty();
	m_write_msgs_queue.push_back(msg);


	if (!write_in_progress)
	{
		boost::asio::async_write(m_socket,
			boost::asio::buffer(m_write_msgs_queue.front().data(),
				m_write_msgs_queue.front().length()),
			boost::bind(&BoostSession::handle_write, shared_from_this(),
				boost::asio::placeholders::error));
	}
}

void BoostSession::handle_read_header(const boost::system::error_code& error)
{
	if (!error && m_read_msg.decode_header())
	{
		boost::asio::async_read(m_socket,
			boost::asio::buffer(m_read_msg.body(), m_read_msg.body_length()),
			boost::bind(&BoostSession::handle_read_body, shared_from_this(),
				boost::asio::placeholders::error));
	}
	else
	{
		m_room.leave(shared_from_this());
	}
}

bool BoostSession::decode_body()
{
	m_read_msg.body()[m_read_msg.body_length()] = 0; // тут нужно слегка отсечь ненужный кусок сообщения
	string msg = m_read_msg.body(); // теперь обрабатываем сообщение через эту тему, а отправляемое оставляем в m_read_msg.body()
	cout << this << ": " << msg << endl;

	return m_room.myServer->HandleMessage(msg, this);
}

void BoostSession::handle_read_body(const boost::system::error_code& error)
{
	if (!error)
	{
		if (decode_body()) {


			//deliver(m_read_msg);
			//m_room.deliver(m_read_msg);

			boost::asio::async_read(m_socket,
				boost::asio::buffer(m_read_msg.data(), message::header_length),
				boost::bind(&BoostSession::handle_read_header, shared_from_this(),
					boost::asio::placeholders::error));
		} else {
			m_room.leave(shared_from_this());
		}
	}
	else
	{
		m_room.leave(shared_from_this());
	}
}

void BoostSession::handle_write(const boost::system::error_code& error)
{
	if (!error)
	{
		m_write_msgs_queue.pop_front();
		if (!m_write_msgs_queue.empty())
		{
			boost::asio::async_write(m_socket,
				boost::asio::buffer(m_write_msgs_queue.front().data(),
					m_write_msgs_queue.front().length()),
				boost::bind(&BoostSession::handle_write, shared_from_this(),
					boost::asio::placeholders::error));
		}
	}
	else
	{
		m_room.leave(shared_from_this());
	}
}

void BoostSession::WriteMsg(const message& msg)
{
}

void BoostSession::WriteLobby(const message& msg, Lobby* lobby)
{
}
