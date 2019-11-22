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
#include "client.h"

using boost::asio::ip::tcp;

typedef std::deque<message> message_queue;


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