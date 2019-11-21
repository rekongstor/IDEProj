#include "session.h"

#include <boost/bind.hpp>

session::session(boost::asio::io_service& io_service, room& room)
	: m_socket(io_service),
	m_room(room)
{
}

tcp::socket& session::socket()
{
	return m_socket;
}

void session::start()
{
	m_room.join(shared_from_this());
	boost::asio::async_read(m_socket,
		boost::asio::buffer(m_read_msg.data(), message::header_length),
		boost::bind(
			&session::handle_read_header, shared_from_this(),
			boost::asio::placeholders::error));
}

void session::deliver(const message& msg)
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

void session::handle_read_header(const boost::system::error_code& error)
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

void session::handle_read_body(const boost::system::error_code& error)
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

void session::handle_write(const boost::system::error_code& error)
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
