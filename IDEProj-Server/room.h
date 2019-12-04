#pragma once

#include "participant.h"
#include <boost/shared_ptr.hpp>
#include <set>

typedef boost::shared_ptr<participant> participant_ptr;

class room
{
public:
	void join(participant_ptr participant);
	void leave(participant_ptr participant);
	void deliver(const message& msg);
	std::set<participant_ptr> m_participants;
private:
};
