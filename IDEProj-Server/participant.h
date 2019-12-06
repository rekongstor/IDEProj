#pragma once

#include "message.hpp"

/// <summary>
/// Абстрактный класс: наследникам можно отправлять сообщения
/// </summary>
class participant
{
public:
	/// <summary>
	/// Виртуальный деструктор абстрактного класса
	/// </summary>
	virtual ~participant() {}
	/// <summary>
	/// Отправить сообщение
	/// </summary>
	/// <param name="msg">Ссылка на сообщение</param>
	virtual void deliver(const message& msg) = 0;
};