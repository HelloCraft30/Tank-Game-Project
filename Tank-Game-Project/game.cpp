#include "game.h"

#define mapOffSet 30

void game::init() {
	//------------------------------------------window
	_window = new sf::RenderWindow(sf::VideoMode(1248, 702), "Tank game", sf::Style::Titlebar);
	_window->setFramerateLimit(60);
	_texture.loadFromFile("png/graphics.png");
	//------------------------------------------player
	player.setSize({ 32,32 });
	player_rect.top = 0; player_rect.left = 0; player_rect.width = 32; player_rect.height = 32;
	player.setTextureRect(player_rect);
	player.setTexture(&_texture);
	player.setPosition({ 0 + mapOffSet,0 + mapOffSet });
	player_dir = 0;
	player_spd = 2.f;
	player_dead = false;
	player_respawn_count_t = 0;
	player_respawn_t = 600;
	player_shoot_cooldown_t = 30;
	player_shoot_cooldown_count_t = 0;
	player_n_bullets = 3;
	player_n_bullets_t = player_n_bullets;
	player_n_bullet_cooldown = 5;
	player_n_bullet_cooldown_t = 15;
	//------------------------------------------bedrock
	bedrock_texture.loadFromFile("png/bedrock.png");
	m_bedrock.setSize({ 32,32 });
	m_bedrock.setTexture(&bedrock_texture);
	//------------------------------------------bullet
	m_bullet.setSize({ 8,8 });
	m_bullet.setFillColor(sf::Color::Red);
	m_bullet.setOrigin({ -12,-12 });
	//------------------------------------------eBullet
	m_eBullet.setSize({ 8,8 });
	m_eBullet.setFillColor(sf::Color::White);
	m_eBullet.setOrigin({ -12,-12 });

	//------------------------------------------enemy
	m_enemy.setSize({ 32,32 });
	m_enemy_rect.top = 0; m_enemy_rect.left = 128; m_enemy_rect.width = 32; m_enemy_rect.height = 32;
	m_enemy.setTextureRect(m_enemy_rect);
	player.setTexture(&_texture);
	enemy_spd = 2.f;

}

void game::loop() {
	while (_window->isOpen()) {
		sf::Event event;
		while (_window->pollEvent(event)) {
			if (event.type == sf::Event::Closed) {
				_window->close();
			}
			//-------------------------------player-event
			if (event.type == sf::Event::KeyPressed) {
				if (!player_dead) {
					if (event.key.code == sf::Keyboard::W) {
						player_movement.y = -player_spd;
					}
					else if (event.key.code == sf::Keyboard::D) {
						player_movement.x = player_spd;
					}
					else if (event.key.code == sf::Keyboard::S) {
						player_movement.y = player_spd;
					}
					else if (event.key.code == sf::Keyboard::A) {
						player_movement.x = -player_spd;
					}
				}
				if (event.key.code == sf::Keyboard::Space) {
					if (player_bullet_ready) {
						player_n_bullets_t = 0;
						player_bullet_ready = false;
					}
				}
				else if (event.key.code == sf::Keyboard::Enter) {
					if (!player_dead) {
						printf("Summoned an enemy.\n");
						enemies.push_back(m_enemy);
						enemies[enemies.size() - 1].setPosition(player.getPosition());
						enemies_dir.push_back(player_dir);
						enemies_rect.push_back(enemies[enemies.size() - 1].getTextureRect());
						enemies_movement.push_back({ 0,0 });
						enemies_random_dir_t.push_back(rand() % 61 + 60);
						enemies_count_t.push_back(0);
						enemies_random_shoot_t.push_back(rand() % 31 + 60);
						enemies_count_shoot_t.push_back(0);
					}
				}
				//only V-H movement
				if (player_movement.x && player_movement.y) player_movement.y = 0;
			}
			if (event.type == sf::Event::KeyReleased) {
				if (event.key.code == sf::Keyboard::W || event.key.code == sf::Keyboard::S) {
					player_movement.y = 0;
				}
				else if (event.key.code == sf::Keyboard::D || event.key.code == sf::Keyboard::A) {
					player_movement.x = 0;
				}
			}
		}
		_window->clear(sf::Color::Black);
		//---------------------------------------Update
		//---------------------------------------Collision
		collision();
		//---------------------------------------Update-bullets
		for (int i = 0; i < bullets.size(); i++) {
			bullets[i].move(bullets_movement[i]);
		}
		//---------------------------------------Update-eBullets
		for (int i = 0; i < eBullets.size(); i++) {
			eBullets[i].move(eBullets_movement[i]);
		}
		//---------------------------------------Update-enemies
		//random dir
		for (int i = 0; i < enemies.size(); i++) {
			if (enemies_count_t[i] >= enemies_random_dir_t[i]) {
				enemies_count_t[i] = 0;
				if (rand() % 2) {
					if (rand() % 2) enemies_movement[i] = { 0, enemy_spd };
					else enemies_movement[i] = { 0, -enemy_spd };
				}
				else {
					if (rand() % 2) enemies_movement[i] = { enemy_spd,0 };
					else enemies_movement[i] = { -enemy_spd ,0 };
				}
			}
			else enemies_count_t[i]++;
		}
		//random shoot
		for (int i = 0; i < enemies.size(); i++) {
			if (enemies_count_shoot_t[i] >= enemies_random_shoot_t[i]) {
				enemies_count_shoot_t[i] = 0;
				eBullets.push_back(m_eBullet);
				eBullets[eBullets.size() - 1].setPosition(enemies[i].getPosition());
				switch (enemies_dir[i]) {
				case 0:
					eBullets_movement.push_back({ 0,-eBullet_spd }); break;
				case 1:
					eBullets_movement.push_back({ eBullet_spd , 0 }); break;
				case 2:
					eBullets_movement.push_back({ 0,eBullet_spd }); break;
				case 3:
					eBullets_movement.push_back({ -eBullet_spd , 0 }); break;
				}
			}
			else enemies_count_shoot_t[i]++;
		}
		//movement
		for (int i = 0; i < enemies.size(); i++) {
			enemies[i].move(enemies_movement[i]);
			if (enemies_movement[i].x) {
				int tmp = round((enemies[i].getPosition().y - mapOffSet) / 32) * 32 + mapOffSet;
				enemies[i].setPosition(enemies[i].getPosition().x, tmp);
			}
			else if (enemies_movement[i].y) {
				int tmp = round((enemies[i].getPosition().x - mapOffSet) / 32) * 32 + mapOffSet;
				enemies[i].setPosition(tmp, enemies[i].getPosition().y);
			}
			if (enemies_movement[i].x > 0 && enemies_movement[i].y == 0) enemies_dir[i] = 1;
			else if (enemies_movement[i].x < 0 && enemies_movement[i].y == 0) enemies_dir[i] = 3;
			else if (enemies_movement[i].y > 0 && enemies_movement[i].x == 0) enemies_dir[i] = 2;
			else if (enemies_movement[i].y < 0 && enemies_movement[i].x == 0) enemies_dir[i] = 0;
		}

		//---------------------------------------Update-player
		if (player_n_bullets_t < player_n_bullets) {
			if (player_n_bullet_cooldown_t >= player_n_bullet_cooldown) {
				player_n_bullet_cooldown_t = 0;
				bullets.push_back(m_bullet);
				bullets[bullets.size() - 1].setPosition(player.getPosition());
				switch (player_dir) {
				case 0:
					bullets_movement.push_back({ 0,-bullet_spd }); break;
				case 1:
					bullets_movement.push_back({ bullet_spd , 0 }); break;
				case 2:
					bullets_movement.push_back({ 0,bullet_spd }); break;
				case 3:
					bullets_movement.push_back({ -bullet_spd , 0 }); break;
				}
				player_n_bullets_t++;
			}
		}

		if (player_n_bullet_cooldown_t < player_n_bullet_cooldown) player_n_bullet_cooldown_t++;
		if (!player_dead) {
			player.move(player_movement);
			if (player_movement.x) {
				int tmp = round((player.getPosition().y - mapOffSet) / 32) * 32 + mapOffSet;
				player.setPosition(player.getPosition().x, tmp);
			}
			else if (player_movement.y) {
				int tmp = round((player.getPosition().x - mapOffSet) / 32) * 32 + mapOffSet;
				player.setPosition(tmp, player.getPosition().y);
			}
			if (player_movement.x > 0 && player_movement.y == 0) player_dir = 1;
			else if (player_movement.x < 0 && player_movement.y == 0) player_dir = 3;
			else if (player_movement.y > 0 && player_movement.x == 0) player_dir = 2;
			else if (player_movement.y < 0 && player_movement.x == 0) player_dir = 0;
		}
		else {
			// respawning
			if (player_respawn_count_t >= player_respawn_t) {
				player_respawn_count_t = 0;
				player_dead = false;
				player.setPosition(player_spawn_point);
			}
			else player_respawn_count_t++;
		}
		if (player_bullet_ready == false) {
			if (player_shoot_cooldown_count_t >= player_shoot_cooldown_t) {
				player_shoot_cooldown_count_t = 0;
				player_bullet_ready = true;
			}
			else player_shoot_cooldown_count_t++;
		}
		draw();
		_window->display();
	}
}

void game::draw() {
	//---------------------------------------------map shape
	sf::RectangleShape mapShape({ 800,640 });
	mapShape.setFillColor(sf::Color::Black);
	mapShape.setOutlineThickness(5);
	mapShape.setOutlineColor(sf::Color::White);
	mapShape.setPosition({ 30,30 });
	_window->draw(mapShape);

	//---------------------------------------------player
	if (!player_dead) {
		switch (player_dir) {
		case 0:
			player_rect.left = 0; break;
		case 1:
			player_rect.left = 32; break;
		case 2:
			player_rect.left = 64; break;
		case 3:
			player_rect.left = 96; break;
		}
		player.setTextureRect(player_rect); player.setTexture(&_texture);
		_window->draw(player);
	}

	//------------------------------------------enemies
	for (int i = 0; i < enemies.size(); i++) {
		switch (enemies_dir[i]) {
		case 0:
			enemies_rect[i].left = 128; break;
		case 1:
			enemies_rect[i].left = 160; break;
		case 2:
			enemies_rect[i].left = 192; break;
		case 3:
			enemies_rect[i].left = 224; break;
		}
		enemies[i].setTextureRect(enemies_rect[i]); enemies[i].setTexture(&_texture);
		_window->draw(enemies[i]);
	}


	//------------------------------------------bedrocks
	for (int i = 0; i < bedrocks.size(); i++) {
		_window->draw(bedrocks[i]);
	}
	//------------------------------------------bullets
	for (int i = 0; i < bullets.size(); i++) {
		_window->draw(bullets[i]);
	}
	//------------------------------------------eBullets
	for (int i = 0; i < eBullets.size(); i++) {
		_window->draw(eBullets[i]);
	}
}

void game::collision() {
	//-------------------- player -------- bedrocks
	for (const auto& br : bedrocks) {
		if (checkCollision(player, br)) {
			printf("Player hit bedrock.\n");
			float distanceX = player.getPosition().x - br.getPosition().x;
			float distanceY = player.getPosition().y - br.getPosition().y;
			if (abs(distanceX) > abs(distanceY)) {
				player_movement.x = 0;
				//fit block
				if (distanceX > 0) {
					player.setPosition(br.getPosition().x + 32, player.getPosition().y);
				}
				else {
					player.setPosition(br.getPosition().x - 32, player.getPosition().y);
				}
			}
			else {
				player_movement.y = 0;
				//fit block
				if (distanceY > 0) {
					player.setPosition(player.getPosition().x, br.getPosition().y + 32);
				}
				else {
					player.setPosition(player.getPosition().x, br.getPosition().y - 32);
				}
			}
		}
	}
	//-------------------- bullet -------- bedrocks
	for (int i = 0; i < bullets.size(); i++) {
		for (const auto& br : bedrocks) {
			if (checkCollision(bullets[i], br)) {
				printf("Bullet hit bedrock.\n");
				bullets.erase(bullets.begin() + i);
				bullets_movement.erase(bullets_movement.begin() + i);
				i = -1;
				break;
			}
		}
	}
	//-------------------- bullet -------- enemy
	for (int i = 0; i < bullets.size(); i++) {
		for (int j = 0; j < enemies.size(); j++) {
			if (checkCollision(bullets[i], enemies[j])) {
				printf("Bullet hit enemy.\n");
				bullets.erase(bullets.begin() + i);
				bullets_movement.erase(bullets_movement.begin() + i);
				i = -1;
				//destroy enemy
				enemies.erase(enemies.begin() + j);
				enemies_dir.erase(enemies_dir.begin() + j);
				enemies_rect.erase(enemies_rect.begin() + j);
				enemies_movement.erase(enemies_movement.begin() + j);
				enemies_random_dir_t.erase(enemies_random_dir_t.begin() + j);
				enemies_count_t.erase(enemies_count_t.begin() + j);
				
				break;
			}
		}
	}
	//-------------------- enemy --------- bedrocks
	for (int i = 0; i < enemies.size(); i++) {
		for (const auto& br : bedrocks) {
			if (checkCollision(enemies[i], br)) {
				printf("Enemy reached hit bedrock. Enemy changed movement\n");
				float distanceX = enemies[i].getPosition().x - br.getPosition().x;
				float distanceY = enemies[i].getPosition().y - br.getPosition().y;
				if (abs(distanceX) > abs(distanceY)) {
					enemies_movement[i].x = 0;
					//fit block
					if (distanceX > 0) {
						enemies[i].setPosition(br.getPosition().x + 32, enemies[i].getPosition().y);
						switch (rand() % 3) {
						case 0:
							enemies_movement[i].x = enemy_spd; break;
						case 1:
							enemies_movement[i].y = enemy_spd; break;
						case 2:
							enemies_movement[i].y = - enemy_spd; break;
						}
					}
					else {
						enemies[i].setPosition(br.getPosition().x - 32, enemies[i].getPosition().y);
						switch (rand() % 3) {
						case 0:
							enemies_movement[i].x = -enemy_spd; break;
						case 1:
							enemies_movement[i].y = enemy_spd; break;
						case 2:
							enemies_movement[i].y = -enemy_spd; break;
						}
					}
				}
				else {
					enemies_movement[i].y = 0;
					//fit block
					if (distanceY > 0) {
						enemies[i].setPosition(enemies[i].getPosition().x, br.getPosition().y + 32);
						switch (rand() % 3) {
						case 0:
							enemies_movement[i].y = enemy_spd; break;
						case 1:
							enemies_movement[i].x = enemy_spd; break;
						case 2:
							enemies_movement[i].x = -enemy_spd; break;
						}
					}
					else {
						enemies[i].setPosition(enemies[i].getPosition().x, br.getPosition().y - 32);
						switch (rand() % 3) {
						case 0:
							enemies_movement[i].y = -enemy_spd; break;
						case 1:
							enemies_movement[i].x = enemy_spd; break;
						case 2:
							enemies_movement[i].x = -enemy_spd; break;
						}
					}
				}

			}
		}
	}
	//-------------------- eBullet ------- player
	for (int i = 0; i < eBullets.size(); i++) {
		if (checkCollision(player, eBullets[i])) {
			eBullets.erase(eBullets.begin() + i);
			eBullets_movement.erase(eBullets_movement.begin() + i);
			i = -1;
			//player dead
			player_dead = true;
			player.setPosition({ 0,0 });
			continue;
		}
	}
	//-------------------- eBullet ------- bedrocks
	for (int i = 0; i < eBullets.size(); i++) {
		for (const auto& br : bedrocks) {
			if (checkCollision(eBullets[i], br)) {
				printf("Bullet hit bedrock.\n");
				eBullets.erase(eBullets.begin() + i);
				eBullets_movement.erase(eBullets_movement.begin() + i);
				i = -1;
				break;
			}
		}
	}
}

void game::map_read_from_file(const std::string& nameF) {
	mapData.resize(0);
	std::fstream fileOpen(nameF, std::ios::in);
	int tmp = 0;
	std::vector<int> tmp_vt;
	int i = 0;
	const int max = 25;
	while (fileOpen.good()) {
		fileOpen >> tmp;
		tmp_vt.push_back(tmp);
		i++;
		if (i == max) {
			i = 0;
			mapData.push_back(tmp_vt);
			tmp_vt.clear();
		}
	}
	fileOpen.close();
}

void game::map_set_game() {
	//------------------------------------------Data-map
	map_read_from_file("level/level_1.txt");
	for (float i = 0; i < 20; i++) {
		for (float j = 0; j < 25; j++) {
			switch (mapData[i][j]) {
			case 0:
				break;
			case 1:
				bedrocks.push_back(m_bedrock);
				bedrocks[bedrocks.size() - 1].setPosition({ j * 32 + mapOffSet, i * 32 + mapOffSet });
				break;
			case 7:
				player.setPosition({ j * 32 + mapOffSet, i * 32 + mapOffSet });
				player_spawn_point = { j * 32 + mapOffSet, i * 32 + mapOffSet };
				break;
			}
		}
	}
}

bool checkCollision(const sf::RectangleShape& a, const sf::RectangleShape& b) {
	return a.getGlobalBounds().intersects(b.getGlobalBounds());

}