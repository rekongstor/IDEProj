#include "room.h"

#include <boost/bind.hpp>
#include <iostream>

using namespace std;

void room::join(participant_ptr participant)
{

	m_participants.insert(participant);
	
	cout << "New member joined: " << participant.get() << endl;
	cout << "Total members joined: " << m_participants.size() << endl;

	if (m_game.m_participant_1 == nullptr)
		m_game.m_participant_1 = participant.get();
	else
		m_game.m_participant_2 = participant.get();
}

void room::leave(participant_ptr participant)
{
	m_participants.erase(participant);

	if (m_game.m_participant_1 == participant.get())
		m_game.m_participant_1 = nullptr;
	else
		m_game.m_participant_2 = nullptr;
}

void room::deliver(const message& msg)
{
	std::for_each(m_participants.begin(), m_participants.end(),
		boost::bind(&participant::deliver, _1, boost::ref(msg)));
}
