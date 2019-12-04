
#include "client.h"


client::client(boost::asio::io_service& io_service,
	tcp::resolver::iterator endpoint_iterator)
	: m_io_service_(io_service),
	m_socket(io_service)
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

	void client::handle_read_body(const boost::system::error_code& error)
	{
		if (!error)
		{
			std::cout.write(m_read_msg.body(), m_read_msg.body_length());
			std::cout << "\n";
			memcpy(status, m_read_msg.body(), m_read_msg.body_length());
			if (status[0] == 'r') {
				std::cout << "Placement completed successfully.";
			}
			if (status[0] == 'h') {
				std::cout << "Successful hit!";
			}
			if (status[0] == 'm') {
				std::cout << "Oops! Missed.";
			}
			if (status[0] == 'f') {
				std::cout << "This is not your turn! Wait...";
			}
			if (status[0] == 'g') {
				std::cout << "The game is over.";
			}
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

	char client::get_status() 
	{
		return status[0];
	}

	void client::do_close()
	{
		m_socket.close();
	}

	void client::gui_draw() {

	}

