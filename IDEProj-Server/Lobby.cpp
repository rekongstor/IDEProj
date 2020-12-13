#include "Lobby.h"

bool Lobby::IsParticipant1(participant* ptr)
{
   return m_participant_1 == ptr;
}

bool Lobby::IsParticipant2(participant* ptr)
{
   return m_participant_2 == ptr;
}

bool Lobby::RemoveParticipant(participant* ptr)
{
   if (m_participant_1 == ptr)
   {
      m_participant_1 = nullptr;
   }
   if (m_participant_2 == ptr)
   {
      m_participant_2 = nullptr;
   }
   if (m_participant_1 || m_participant_2)
      return true;
   return false;
}

bool Lobby::AddParticipant(participant* ptr)
{
   if (!m_participant_1)
   {
      m_participant_1 = ptr;
      return true;
   }
   if (!m_participant_2 )
   {
      m_participant_2 = ptr;
      return true;
   }
   return false;
}
