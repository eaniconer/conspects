#include "Door.h"

Door::Door(Room* room1, Room* room2) : _room1(room1), _room2(room2)
{
	_isOpen = true;
}

void Door::Enter()
{

}

Room* Door::OtherSideFromRoom(Room* room) 
{
	if (_room1 == room) 
	{
		return _room2;
	}

	if (_room2 == room) 
	{
		return _room1;
	}

	return nullptr;
}