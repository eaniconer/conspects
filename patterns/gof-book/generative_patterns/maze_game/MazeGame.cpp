#include "MazeGame.h"

#include <iostream>

Maze* MazeGame::CreateMaze() {
	Maze* aMaze = new Maze();

	Room* r1 = new Room(1);
	Room* r2 = new Room(2);
	Door* theDoor = new Door(r1, r2);

	aMaze->AddRoom(r1);
	std::cout << "r1: " << r1 << std::endl;
	std::cout << "getByIndex(1): " << aMaze->GetRoomByIndex(1) << std::endl;

	aMaze->AddRoom(r2);

	r1->SetSide(North, new Wall());
	r1->SetSide(East, theDoor);
	r1->SetSide(South, new Wall());
	r1->SetSide(West, new Wall());

	r1->SetSide(North, new Wall());
	r1->SetSide(East, new Wall());
	r1->SetSide(South, new Wall());
	r1->SetSide(West, theDoor);

	return aMaze;
}