#pragma once

#include <cstdio>
#include <cstdlib>
#include <cstring>

/// <summary>
/// Класс сообщения
/// </summary>
class message
{
public:
	enum { header_length = 4 };
	enum { max_body_length = 512 };

	message()
		: body_length_(0)
	{
	}
	/// <summary>
	/// Возвращает поле data_ из сообщения как cnost char*
	/// </summary>
	const char* data() const
	{
		return data_;
	}
	/// <summary>
	/// Возвращает поле data_ из сообщения как char* 
	/// </summary>
	char* data()
	{
		return data_;
	}
	/// <summary>
	/// Возвращает длину сообщения
	/// </summary>
	size_t length() const
	{
		return header_length + body_length_;
	}
	/// <summary>
	/// Возвращает const char* указатель на текст сообщения
	/// </summary>
	const char* body() const
	{
		return data_ + header_length;
	}
	/// <summary>
	/// Возвращает char* указатель на текст сообщения
	/// </summary>
	char* body()
	{
		return data_ + header_length;
	}
	/// <summary>
	/// Возвращает длину текста сообщения
	/// </summary>
	size_t body_length() const
	{
		return body_length_;
	}
	/// <summary>
	/// Устанавливает длину текста сообщения
	/// </summary>
	void body_length(size_t new_length)
	{
		body_length_ = new_length;
		if (body_length_ > max_body_length)
			body_length_ = max_body_length;
	}
	/// <summary>
	/// Декодирование сообщения из заголовка
	/// </summary>
	bool decode_header()
	{
		using namespace std; // For strncat and atoi.
		char header[header_length + 1] = "";
		strncat(header, data_, header_length);
		body_length_ = atoi(header);
		if (body_length_ > max_body_length)
		{
			body_length_ = 0;
			return false;
		}
		return true;
	}
	/// <summary>
	/// Кодирование заголовка из сообщения
	/// </summary>
	void encode_header()
	{
		using namespace std; // For sprintf and memcpy.
		char header[header_length + 1] = "";
		sprintf(header, "%4d", static_cast<int>(body_length_));
		memcpy(data_, header, header_length);
	}

private:
	char data_[header_length + max_body_length];
	size_t body_length_;
};
