#include <iostream>
#include "Game.hpp"

gm::Game::Game() : _window(sf::VideoMode(640, 480), "SFML App"), _texture(), _player()
{
	if(!_texture.loadFromFile("images/plane.png"))
	{
		std::cout << "Bad image";
	}
	_player.setTexture(_texture);
	_player.setPosition(100.f, 100.f);
}

void gm::Game::Run()
{
	const sf::Time TimePerFrame = sf::seconds(1.f / 60.f);
	sf::Clock clock;
	sf::Time timeSinceLastUpdate = sf::Time::Zero;
	while(_window.isOpen()) 
	{
		ProcessEvents();
		timeSinceLastUpdate += clock.restart();
		while(timeSinceLastUpdate > TimePerFrame) {
			timeSinceLastUpdate -= TimePerFrame;
			ProcessEvents();
			Update(TimePerFrame);
		}
		Render();
	}
}

void gm::Game::ProcessEvents() 
{
	sf::Event event;
	while(_window.pollEvent(event))
	{
		switch(event.type) {		
			case sf::Event::KeyPressed:
				HandlePlayerInput(event.key.code, true);
				break;
			case sf::Event::KeyReleased:
				HandlePlayerInput(event.key.code, false);
				break;
			case sf::Event::Closed:
				_window.close();
				break;
			default:
				break;
		}
	}
}

void gm::Game::Update(const sf::Time& deltaTime) 
{
	float speed = 60.f;
	sf::Vector2f movement(0.f, 0.f);
	if(_isMovingUp) 	movement.y -= speed;
	if(_isMovingDown) 	movement.y += speed;
	if(_isMovingLeft)	movement.x -= speed;
	if(_isMovingRight)	movement.x += speed;
	_player.move(movement * deltaTime.asSeconds());	
}

void gm::Game::Render() 
{
	_window.clear();
	_window.draw(_player);
	_window.display();
}

void gm::Game::HandlePlayerInput(sf::Keyboard::Key key, bool isPressed) 
{
	switch(key) 
	{
		case sf::Keyboard::W:
			_isMovingUp = isPressed;
			break;
		case sf::Keyboard::S:
			_isMovingDown = isPressed;
			break;
		case sf::Keyboard::A:
			_isMovingLeft = isPressed;
			break;
		case sf::Keyboard::D:
			_isMovingRight = isPressed;
			break;
		default:
			break;
	}
}