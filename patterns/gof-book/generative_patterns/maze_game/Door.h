#pragma once
#ifndef DOOR_H
#define DOOR_H

#include "MapSite.h"
#include "Room.h"

class Door : public MapSite
{
public:
	Door(Room* room1 = nullptr, Room* room2 = nullptr);

	virtual void Enter();	
	Room* OtherSideFromRoom(Room*);

private:
	Room* _room1;
	Room* _room2;
	bool _isOpen;
};

#endif // DOOR_H