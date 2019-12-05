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

	auto game_finished = [&](field* f)
	{
		for (int x = 0; x < 10; ++x)
			for (int y = 0; y < 10; ++y)
				if ((f->get_cell(x, y) & cell::ship) && !(f->get_cell(x, y) & cell::shot)) // есть неподстрелянный корабль
					return false;
		return true;
	};

	auto swap_state = [&]()
	{
		if (m_room.m_game.m_participant_1 == this)
			m_room.m_game.set_state(game::egs::turn_2);
		else
			m_room.m_game.set_state(game::egs::turn_1);
	};

	m_read_msg.body()[m_read_msg.body_length()] = 0; // тут нужно слегка отсечь ненужный кусок сообщения
	string msg = m_read_msg.body(); // теперь обрабатываем сообщение через эту тему, а отправляемое оставляем в m_read_msg.body()
	cout << this << ": " << msg << endl;

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

				if (!my_field->set_ship(msg[2] - '0', msg[4] - '0', msg[6] - '0', msg[8]))
				{
					set_msg("f");
					cout << "Placement error"<< endl;
					deliver(m_read_msg);
					return;
				}

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
					set_msg("t");
					deliver(m_read_msg);
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
		cout << "Turns " << msg.size() << endl;
		if ((m_room.m_game.get_state() == game::egs::turn_1 && m_room.m_game.m_participant_1 == this) || \
			(m_room.m_game.get_state() == game::egs::turn_2 && m_room.m_game.m_participant_2 == this))
		{
			if (msg.size() == 3)
				if (msg[0] >= '0' && msg[0] <= '9' && \
					msg[1] == ' ' && \
					msg[2] >= '0' && msg[2] <= '9')
				{
					field* my_field;
					if (m_room.m_game.m_participant_1 == this)
						my_field = &m_room.m_game.m_field_2;
					else
						my_field = &m_room.m_game.m_field_1;

					int x = msg[0] - '0';
					int y = msg[2] - '0';
					int c = my_field->get_cell(x, y);
					my_field->set_cell(x, y, c | cell::shot);


					if (c & cell::shot)
					{
						cout << "Turn repeat - failed " << msg << endl;
						set_msg("f"); // выстрел туда же?
						deliver(m_read_msg);
						return;
					}

					if (c & cell::ship)
					{
						cout << "Hit! " << msg << endl;
						char sh[] = "h x y";
						sh[2] = '0' + x;
						sh[4] = '0' + y;
						set_msg(sh); // попадание

						if (game_finished(my_field))
						{
							cout << "GG " << msg << endl;
							set_msg("g"); // победа
							m_room.deliver(m_read_msg);
							m_room.m_game.set_state(game::egs::end);
							return;
						}

						m_room.deliver(m_read_msg);
						return;
					}
					else
					{
						cout << "Miss " << msg << endl;
						char sh[] = "m x y";
						sh[2] = '0' + x;
						sh[4] = '0' + y;
						set_msg(sh); // попадание
						swap_state();
						m_room.deliver(m_read_msg);
						return;
					}
				}
		}
		else
		{
			cout << "Turn order - failed " << msg << endl;
			set_msg("f"); // ход не в своё время
			deliver(m_read_msg);
			return;
		}


		m_room.deliver(m_read_msg);
		return;
	}

	if (m_room.m_game.get_state() == game::egs::end)
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
