
#include "BoostClient.h"
#include <string>


BoostClient::BoostClient(boost::asio::io_service& io_service,
	tcp::resolver::iterator endpoint_iterator)
	: m_io_service_(io_service),
	m_socket(io_service)
{
	time_t t;
	time(&t);
	id = t;
	boost::asio::async_connect(m_socket, endpoint_iterator,
		boost::bind(&BoostClient::handle_connect, this,
			boost::asio::placeholders::error));
}

void BoostClient::write(const message& msg)
{
	std::string dbgMsg(msg.body(), msg.body_length());
	OutputDebugString("Client ");
	OutputDebugString(std::to_string(id).c_str());
	OutputDebugString(" delivers message: ");
	OutputDebugString(dbgMsg.c_str());
	OutputDebugString("\n");

	m_io_service_.post(boost::bind(&BoostClient::do_write, this, msg));
}

void BoostClient::close()
{
	m_io_service_.post(boost::bind(&BoostClient::do_close, this));
}

void BoostClient::handle_connect(const boost::system::error_code& error)
{
	if (!error)
	{
		boost::asio::async_read(m_socket,
			boost::asio::buffer(m_read_msg.data(), message::header_length),
			boost::bind(&BoostClient::handle_read_header, this,
				boost::asio::placeholders::error));
	}
}

void BoostClient::handle_read_header(const boost::system::error_code& error)
{
	if (!error && m_read_msg.decode_header())
	{
		boost::asio::async_read(m_socket,
			boost::asio::buffer(m_read_msg.body(), m_read_msg.body_length()),
			boost::bind(&BoostClient::handle_read_body, this,
				boost::asio::placeholders::error));
	}
	else
	{
		do_close();
	}
}

void BoostClient::decode_message()
{
	using namespace std;
	
	string msg = m_read_msg.body();

   OutputDebugString("Client ");
   OutputDebugString(std::to_string(id).c_str());
   OutputDebugString(" receives message: ");
	OutputDebugString(msg.c_str());
	OutputDebugString("\n");

	HandleMessage(msg);
}

void BoostClient::handle_read_body(const boost::system::error_code& error)
{
	if (!error)
	{
		decode_message();
		//std::cout.write(m_read_msg.body(), m_read_msg.body_length());
		//std::cout << "\n";


		boost::asio::async_read(m_socket,
			boost::asio::buffer(m_read_msg.data(), message::header_length),
			boost::bind(&BoostClient::handle_read_header, this,
				boost::asio::placeholders::error));
	}
	else
	{
		do_close();
	}
}

void BoostClient::do_write(message msg)
{
	bool write_in_progress = !m_write_msg_queue.empty();
	m_write_msg_queue.push_back(msg);
	if (!write_in_progress)
	{
		boost::asio::async_write(m_socket,
			boost::asio::buffer(m_write_msg_queue.front().data(),
				m_write_msg_queue.front().length()),
			boost::bind(&BoostClient::handle_write, this,
				boost::asio::placeholders::error));
	}
}

void BoostClient::handle_write(const boost::system::error_code& error)
{
	if (!error)
	{
		m_write_msg_queue.pop_front();
		if (!m_write_msg_queue.empty())
		{
			boost::asio::async_write(m_socket,
				boost::asio::buffer(m_write_msg_queue.front().data(),
					m_write_msg_queue.front().length()),
				boost::bind(&BoostClient::handle_write, this,
					boost::asio::placeholders::error));
		}
	}
	else
	{
		do_close();
	}
}
	
void BoostClient::do_close()
{
	m_socket.close();
}

void BoostClient::gui_draw() {

}

