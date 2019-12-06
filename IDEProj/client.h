﻿#pragma once

#include <cstdlib>
#include <deque>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>

#include "message.hpp"
#include "client_gui_console.h"
#include "field.hpp"

using boost::asio::ip::tcp;
typedef std::deque<message> message_queue;


class client
{
public:
	/// <summary>
	/// Конструктор создания классов.
	/// </summary>
	/// <param name="io_service">Ссылка на созданный io_service</param>
	/// <param name="endpoint_iterator">Ссылка на endpoint_iterator</param>
	client(boost::asio::io_service& io_service, tcp::resolver::iterator endpoint_iterator);
	/// <summary>
	/// Метод для отправки сообщения
	/// </summary>
	/// <param name="msg">Ссылка на сообщение</param>
	void write(const message& msg);
	/// <summary>
	/// Метод, вызываемый перед закрытием подключения
	/// </summary>
	void close();
	/// <summary>
	/// Объект интерфейса
	/// </summary>
	client_gui_console m_gui;
	/// <summary>
	/// Переменная состояний игры
	/// </summary>
	client_gui_console::egs state;
	/// <summary>
	/// Готовность к приёму сообщений
	/// </summary>
	bool receive;
	/// <summary>
	/// Переменная, отвечающая за готовность расстановки кораблей
	/// </summary>
	bool ready;
	/// <summary>
	/// Переменная, отвечающая за порядок ходов: true - игрок ходит вторым, false - игрок ходит первым
	/// </summary>
	bool second_turn;
	/// <summary>
	/// Игровое поле игрока
	/// </summary>
	field m_my;
	/// <summary>
	/// Игрокове поле соперника
	/// </summary>
	field m_en;
private:

	void handle_connect(const boost::system::error_code& error);
	void handle_read_header(const boost::system::error_code& error);
	void handle_read_body(const boost::system::error_code& error);
	void decode_message();
	void do_write(message msg);
	void handle_write(const boost::system::error_code& error);
	void do_close();
	void gui_draw();

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
