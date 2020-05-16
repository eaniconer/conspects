#pragma once
#ifndef ROOM_H
#define ROOM_H

#include "MapSite.h"

class Room : public MapSite
{
public:
	Room(int roomIndex);

	MapSite* GetSide(Direction) const;
	void SetSide(Direction, MapSite*);
	int GetIndex() const;

	virtual void Enter();

private:
	MapSite* _sides[4];
	int _roomIndex;
};

#endif // ROOM_H