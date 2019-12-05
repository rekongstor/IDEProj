
#include "client.h"
#include <string>


client::client(boost::asio::io_service& io_service,
	tcp::resolver::iterator endpoint_iterator)
	: m_io_service_(io_service),
	m_socket(io_service), m_gui(&m_my, &m_en), receive(false), ready(false), second_turn(false)
{
	boost::asio::async_connect(m_socket, endpoint_iterator,
		boost::bind(&client::handle_connect, this,
			boost::asio::placeholders::error));
}

void client::write(const message& msg)
{
	m_io_service_.post(boost::bind(&client::do_write, this, msg));
}

void client::close()
{
	m_io_service_.post(boost::bind(&client::do_close, this));
}

	void client::handle_connect(const boost::system::error_code& error)
	{
		if (!error)
		{
			boost::asio::async_read(m_socket,
				boost::asio::buffer(m_read_msg.data(), message::header_length),
				boost::bind(&client::handle_read_header, this,
					boost::asio::placeholders::error));
		}
	}

void client::handle_read_header(const boost::system::error_code& error)
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

void client::decode_message()
{
	using namespace std;
	
	string msg = m_read_msg.body();


	if (m_gui.get_state() == client_gui_console::egs::preparation) // we're ereceiving placement messages
	{
		if (ready == true)
		{
			if (msg == "t") // our turn is second
			{
				second_turn = true;
				return;
			}
			if (msg == "d") // Usual 
			{
				receive = false;
				if (second_turn)
					m_gui.set_state(client_gui_console::egs::enemy_turn);
				else
					m_gui.set_state(client_gui_console::egs::my_turn);

				return;
			}
		}
		else
		{
			if (msg == "d") // Usual 
			{
				receive = false;
				return;
			}
			if (msg == "f") // shouldn't happen
			{
				receive = false;
				cout << "Server error happened. This is really bad." << endl;
				return;
			}
		}
		return;
	}
	if (m_gui.get_state() == client_gui_console::egs::my_turn) // we'r ereceiving my_turn messages
	{

		if (msg[0] == 'h')
		{
			m_en.set_cell(msg[2] - '0', msg[4] - '0', cell::ship | cell::shot);
			receive = false; // продолжаем стрел€ть
		}
		if (msg[0] == 'm')
		{
			m_en.set_cell(msg[2] - '0', msg[4] - '0', cell::shot);
			m_gui.set_state(client_gui_console::egs::enemy_turn); 
			receive = false; // передаЄм ход сопернику
		}
		return;
	}
	if (m_gui.get_state() == client_gui_console::egs::enemy_turn) // we'r ereceiving enemy_turn messages
	{
		if (msg[0] == 'h') 
		{
			m_my.set_cell(msg[2] - '0', msg[4] - '0', cell::ship | cell::shot);
			receive = false; // продолжаем стрел€ть
		}
		if (msg[0] == 'm')
		{
			m_my.set_cell(msg[2] - '0', msg[4] - '0', cell::shot);
			m_gui.set_state(client_gui_console::egs::my_turn);
			receive = false; // передаЄм ход сопернику
		}
		if (msg[0] == 'f') // не должно происходить
		{
			cout << "Not your turn"; 
			receive = false; // продолжаем стрел€ть
		}
		if (msg[0] == 'g') // не должно происходить
		{
			m_gui.set_state(client_gui_console::egs::end);
			receive = false; // продолжаем стрел€ть
		}
		return;
	}
	if (m_gui.get_state() == client_gui_console::egs::end) // we'r ereceiving end messages
	{
		receive = false; // продолжаем стрел€ть
		return;
	}
	//if (status[0] == 'r') {
	//	std::cout << "Placement completed successfully.";
	//}
	//if (status[0] == 'h') {
	//	std::cout << "Successful hit!";
	//}
	//if (status[0] == 'm') {
	//	std::cout << "Oops! Missed.";
	//}
	//if (status[0] == 'f') {
	//	std::cout << "This is not your turn! Wait...";
	//}
	//if (status[0] == 'g') {
	//	std::cout << "The game is over.";
	//}
	//if (status[0] == 't') {
	//	std::cout << "The opponentТs move. Wait...";
	//}
}

void client::handle_read_body(const boost::system::error_code& error)
{
	if (!error)
	{
		decode_message();
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

void client::do_write(message msg)
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

void client::handle_write(const boost::system::error_code& error)
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
	
void client::do_close()
{
	m_socket.close();
}

void client::gui_draw() {

}

