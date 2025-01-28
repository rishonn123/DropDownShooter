#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <sstream>

// Set screen size
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

// Set bullet speed
const float BULLET_SPEED = 500.0f;

// Set dinosaur speed
const float DINO_SPEED = 100.0f;

struct Bullet {
    sf::Sprite sprite;
    bool active = false;
};

struct Dinosaur {
    sf::Sprite sprite;
    bool active = true;
};

int main() {
    //Set screen
    sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Jurassic Shooter Game");
    window.setFramerateLimit(60);

    // Seed random number generator
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    // Create background
    sf::Sprite background;
    sf::Texture backgroundTexture;
    if (!backgroundTexture.loadFromFile("images/background.png")) {
        std::cerr << "Error loading background texture!" << std::endl;
        return -1;
    }
    background.setTexture(backgroundTexture);

    // Tank setup
    sf::Sprite tank;
    sf::Texture tankTexture;
    if (!tankTexture.loadFromFile("images/tank.png")) {
        std::cerr << "Error loading tank texture!" << std::endl;
        return -1;
    }
    tank.setTexture(tankTexture);
    tank.setPosition(SCREEN_WIDTH / 2 - tankTexture.getSize().x / 2, SCREEN_HEIGHT - tankTexture.getSize().y - 10);

    // Bullet setup
    Bullet bullet;
    sf::Texture bulletTexture;
    if (!bulletTexture.loadFromFile("images/bullet.png")) {
        std::cerr << "Error loading bullet texture!" << std::endl;
        return -1;
    }
    bullet.sprite.setTexture(bulletTexture);

    // Dinosaur setup
    sf::Texture dinoTexture;
    if (!dinoTexture.loadFromFile("images/dinosaur.png")) {
        std::cerr << "Error loading dinosaur texture!" << std::endl;
        return -1;
    }

    std::vector<Dinosaur> dinosaurs;
    
    //Clock for dinosaur spawning
    sf::Clock spawnClock;  
    sf::Clock gameClock;

    bool isGameOver = false;

    // Setting up score
    int score = 0;
    sf::Text scoreText;
    sf::Font font;
    if (!font.loadFromFile("images/arial.ttf")) {
        std::cerr << "Error loading font!" << std::endl;
        return -1;
    }
    scoreText.setFont(font);
    scoreText.setCharacterSize(24);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition(10, 10);

    while (window.isOpen()) {
        
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        if (isGameOver) {
            window.clear();

            sf::Text gameOverText;
            gameOverText.setFont(font);
            gameOverText.setString("Game Over!\nScore: " + std::to_string(score));
            gameOverText.setFillColor(sf::Color::Red);
            gameOverText.setCharacterSize(50);
            gameOverText.setPosition(SCREEN_WIDTH / 2 - 150, SCREEN_HEIGHT / 2 - 50);
            window.draw(gameOverText);
            window.display();
            continue;
        }

        
        float deltaTime = gameClock.restart().asSeconds();

        // Tank movement left/right
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) && tank.getPosition().x > 0) {
            tank.move(-300.0f * deltaTime, 0);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) && tank.getPosition().x + tankTexture.getSize().x < SCREEN_WIDTH) {
            tank.move(300.0f * deltaTime, 0);
        }

        // Shooting bullets spacebar
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && !bullet.active) {
            bullet.active = true;
            bullet.sprite.setPosition(tank.getPosition().x + tankTexture.getSize().x / 2 - bulletTexture.getSize().x / 2, tank.getPosition().y - bulletTexture.getSize().y);
        }

        // Moving bullets
        if (bullet.active) {
            bullet.sprite.move(0, -BULLET_SPEED * deltaTime);
            if (bullet.sprite.getPosition().y + bulletTexture.getSize().y < 0) {
                bullet.active = false;
            }
        }

        //Dinosaur spawner
        if (spawnClock.getElapsedTime().asSeconds() > 1.0f) {
            spawnClock.restart();
            Dinosaur dino;
            dino.sprite.setTexture(dinoTexture);
            dino.sprite.setPosition(std::rand() % (SCREEN_WIDTH - dinoTexture.getSize().x), 0);
            dinosaurs.push_back(dino);
        }

        // Moving dinosaurs
        for (auto& dino : dinosaurs) {
            dino.sprite.move(0, DINO_SPEED * deltaTime);
            if (dino.sprite.getPosition().y > SCREEN_HEIGHT) {
                isGameOver = true;
            }
        }
        // Check if Dinosaur hit
        for (auto& dino : dinosaurs) {
            if (bullet.active && dino.active && bullet.sprite.getGlobalBounds().intersects(dino.sprite.getGlobalBounds())) {
                bullet.active = false;
                dino.active = false;
                score += 10;
            }
        }
        // Remove dinosaurs
        dinosaurs.erase(
            std::remove_if(dinosaurs.begin(), dinosaurs.end(), [](const Dinosaur& dino) { return !dino.active; }),
            dinosaurs.end());

        // Update score text
        std::ostringstream scoreStream;
        scoreStream << "Score: " << score;
        scoreText.setString(scoreStream.str());

       
        window.clear();

        window.draw(background);
        window.draw(tank);

        if (bullet.active) {
            window.draw(bullet.sprite);
        }
        for (const auto& dino : dinosaurs) {
            window.draw(dino.sprite);
        }

        window.draw(scoreText);

        window.display();
    }

    return 0;
}
