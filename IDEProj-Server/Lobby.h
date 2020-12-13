#pragma once
#include "Game.h"
class participant;

class Lobby
{
public:
	Game m_game;
	
	participant* m_participant_1;
	participant* m_participant_2;

   bool IsParticipant1(participant* ptr);
   bool IsParticipant2(participant* ptr);

	bool AddParticipant(participant* ptr); // Returns true if succeed
	bool RemoveParticipant(participant* ptr); // Returns true if lobby is non-empty
};

