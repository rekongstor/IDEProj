#include "session.h"

#include <boost/bind.hpp>
#include <iostream>
using namespace std;

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
	if (m_room.m_participants.size() > 1)
	{
		cout << "Member cannot join: " << shared_from_this() << endl;
		return;
	}
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

void session::decode_body()
{
	auto set_msg = [&](const char* line)
	{
		m_read_msg.body_length(strlen(line));
		memcpy(m_read_msg.body(), line, m_read_msg.body_length());
		m_read_msg.encode_header();
	};

	m_read_msg.body()[m_read_msg.body_length()] = 0; // ��� ����� ������ ������ �������� ����� ���������
	string msg = m_read_msg.body(); // ������ ������������ ��������� ����� ��� ����, � ������������ ��������� � m_read_msg.body()
	
	if (m_room.m_game.get_state() == game::egs::preparation)
	{
		cout << "Preparation " << msg.size() << endl;
		if (msg.size() == 9)
			if (msg[0] == 's' && \
				msg[1] == ' ' && \
				msg[2] >= '0' && msg[2] <= '9' && \
				msg[3] == ' ' && \
				msg[4] >= '0' && msg[4] <= '9' && \
				msg[5] == ' ' && \
				msg[6] >= '1' && msg[6] <= '4' && \
				msg[7] == ' ' && \
				(msg[8] == 'v' || msg[8] == 'h'))
			{
				field* my_field;
				if (m_room.m_game.m_participant_1 == this)
					my_field = &m_room.m_game.m_field_1;
				else
					my_field = &m_room.m_game.m_field_2;

				my_field->set_ship(msg[2] - '0', msg[4] - '0', msg[6] - '0', msg[8]);

				set_msg("d");
				deliver(m_read_msg);
				return;
			}
		if (msg.size() == 1)
			if (msg[0] == 'r')
			{
				if (m_room.m_game.m_participant_1 == this)
					m_room.m_game.m_p1_ready = true;

				if (m_room.m_game.m_participant_2 == this)
					m_room.m_game.m_p2_ready = true;

				if (m_room.m_game.m_p1_ready && m_room.m_game.m_p2_ready)
				{
					set_msg("d");
					m_room.m_game.set_state(game::egs::turn_1);
					m_room.deliver(m_read_msg);

					for (int y = 0; y < 10; ++y)
					{
						for (int x = 0; x < 10; ++x)
							cout << m_room.m_game.m_field_1.get_cell(x, y) << ' ';
						cout << endl;
					}
					cout << endl;

					for (int y = 0; y < 10; ++y)
					{
						for (int x = 0; x < 10; ++x)
							cout << m_room.m_game.m_field_2.get_cell(x, y) << ' ';
						cout << endl;
					}
					cout << endl;

					return;
				}
				return;
			}
	}
	if (m_room.m_game.get_state() == game::egs::turn_1 || m_room.m_game.get_state() == game::egs::turn_2)
	{
		m_room.deliver(m_read_msg);
		return;
	}

	set_msg("f");
	deliver(m_read_msg);
	cout << this << ": " << msg << endl;
}

void session::handle_read_body(const boost::system::error_code& error)
{
	if (!error)
	{
		decode_body();

		//deliver(m_read_msg);
		//m_room.deliver(m_read_msg);

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
