#include "Room.h"

Room::Room(int roomIndex)
{
	_roomIndex = roomIndex;
	_sides[0] = nullptr;
	_sides[1] = nullptr;
	_sides[2] = nullptr;
	_sides[3] = nullptr;
}

MapSite* Room::GetSide(Direction direction) const
{
	return _sides[(int)direction];
}

void Room::SetSide(Direction direction, MapSite* site) 
{
	_sides[(int)direction] = site;
}

void Room::Enter() 
{

}

int Room::GetIndex() const
{
	return _roomIndex;
}