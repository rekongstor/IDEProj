#include "BoostRoom.h"

#include <boost/bind.hpp>
#include <iostream>

using namespace std;

void BoostRoom::join(participant_ptr participant)
{
	m_participants.insert(participant);
	
	cout << "New participant joined: " << participant.get() << endl;
	cout << "Total participants: " << m_participants.size() << endl;

	myServer->clentData[participant.get()] = SharedClient();
}

void BoostRoom::leave(participant_ptr participant)
{
	
	cout << "Participant left: " << participant.get() << endl;
	cout << "Total participants: " << m_participants.size() << endl;
	m_participants.erase(participant);

	myServer->clentData.erase(participant.get());
}

void BoostRoom::deliver(const message& msg)
{
	std::for_each(m_participants.begin(), m_participants.end(),
		boost::bind(&participant::deliver, _1, boost::ref(msg)));
}

void BoostRoom::deliver(const message& msg, void* participantPtr)
{
	participant* pPart = (participant*)participantPtr;
	pPart->deliver(msg);
}
