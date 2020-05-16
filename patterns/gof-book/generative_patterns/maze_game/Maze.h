#pragma once
#ifndef MAZE_H
#define MAZE_H

#include <map>

#include "Room.h"
#include "Door.h"
#include "Wall.h"

class Maze 
{
public:
	Maze();
	void AddRoom(Room*);
	Room* GetRoomByIndex(int) const;
private:
	std::map<int, Room*> _roomIndex2room;
};

#endif // MAZE_H