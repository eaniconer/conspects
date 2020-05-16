#include "Maze.h"

Maze::Maze() 
{
	// ctor implementation
}

void Maze::AddRoom(Room* room) 
{
	_roomIndex2room[room->GetIndex()] = room;
}

Room* Maze::GetRoomByIndex(int roomIndex) const
{
	if (_roomIndex2room.find(roomIndex) != _roomIndex2room.cend())
	{
		return _roomIndex2room.at(roomIndex);
	}
	return nullptr;
}