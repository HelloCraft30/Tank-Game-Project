#pragma once
#include<SFML/Graphics.hpp>
#include <vector>
#include<fstream>
#include <iostream>

#define SIZEMAP 640

bool checkCollision(const sf::RectangleShape& a, const sf::RectangleShape& b);

class game
{
public:
	void init();

	void loop();

	void draw();

	void collision();

	//------------------------------------------Window

	sf::RenderWindow* _window;

	sf::Texture _texture;

	//------------------------------------------Data-map
	std::vector<std::vector<int>> mapData;
	void map_read_from_file(const std::string& nameFile);

	void map_set_game();

	//------------------------------------------player
	bool player_dead;
	sf::RectangleShape player;
	sf::IntRect player_rect;
	int player_dir;
	float player_spd;
	sf::Vector2f player_movement;

	int player_respawn_t;
	int player_respawn_count_t;

	sf::Vector2f player_spawn_point;

	int player_n_bullets;
	int player_n_bullets_t;
	int player_n_bullet_cooldown;
	int player_n_bullet_cooldown_t;

	bool player_bullet_ready;
	int player_shoot_cooldown_t;
	int player_shoot_cooldown_count_t;

	//------------------------------------------bullet
	std::vector<sf::RectangleShape> bullets;
	std::vector<sf::Vector2f> bullets_movement;
	float bullet_spd = 8.f;

	sf::RectangleShape m_bullet;
	//------------------------------------------Enemy-Bullet
	std::vector<sf::RectangleShape> eBullets;
	std::vector<sf::Vector2f> eBullets_movement;
	float eBullet_spd = 6.f;

	sf::RectangleShape m_eBullet;

	//------------------------------------------enemy
	std::vector<sf::RectangleShape> enemies;
	std::vector<sf::IntRect> enemies_rect;
	std::vector<int> enemies_dir;
	std::vector<sf::Vector2f> enemies_movement;
	float enemy_spd;
	//randome dir
	std::vector<int> enemies_random_dir_t;
	std::vector<int> enemies_count_t;
	//random shoot
	std::vector<int> enemies_random_shoot_t;
	std::vector<int> enemies_count_shoot_t;

	sf::RectangleShape m_enemy;
	sf::IntRect m_enemy_rect;

	//------------------------------------------Block
	//------------------------------------------Grass
	std::vector<sf::RectangleShape> grasses;
	sf::RectangleShape m_grass;
	sf::Texture grass_texture;

	//------------------------------------------Wall
	std::vector<sf::RectangleShape> walls;
	//------------------------------------------Bedrock
	std::vector<sf::RectangleShape> bedrocks;
	sf::RectangleShape m_bedrock;
	sf::Texture bedrock_texture;
};

