#pragma once
#ifndef GAME_H
#define GAME_H

#include <SFML/Graphics.hpp>

namespace gm
{
	class Game
	{
	public:
				Game();
		void 	Run();

	private:
		void 	ProcessEvents();
		void 	Update(const sf::Time&);
		void 	Render();

		void 	HandlePlayerInput(sf::Keyboard::Key key, bool isPressed);

	private:
		sf::RenderWindow	_window;

		sf::Texture 		_texture;
		sf::Sprite	 		_player;

		bool _isMovingUp = false;
		bool _isMovingDown = false;
		bool _isMovingLeft = false;
		bool _isMovingRight = false;
	};
}

#endif //GAME_H