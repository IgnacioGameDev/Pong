#include <SFML/Graphics.hpp>
#include <iostream>
#include "PongEnemy.h"

static const int windowX = 1920;
static const int windowY = 1080;

static const float collisionOffsetLeft = 50;
static const float collisionOffsetRight = 60;
static const float collisionOffsetTop = 120;
static const float collisionOffsetBottom = 110;

static const sf::Color backgroundColor = sf::Color(158, 158, 158, 255);
static const sf::Vector2f paddleSize = sf::Vector2f(30, 200);
static const float ballSize = 30;
static const sf::Color playerPaddleColor = sf::Color::Blue;
static const sf::Color enemyPaddleColor = sf::Color::Red;
static const sf::Color ballColor = sf::Color::Black;

static const sf::Vector2f startingBallSpeed = sf::Vector2f(-1100, 200);

struct Ball {
    sf::CircleShape body;
    sf::Vector2f speed;

    Ball() {
        body = sf::CircleShape(ballSize);
        body.setPosition(windowX / 2, windowY / 2);
        body.setFillColor(ballColor);
        speed = startingBallSpeed;
    }
};

struct Paddle {
    bool isPlayer;
    sf::RectangleShape body;

    Paddle(bool isPlayer_) {
        isPlayer = isPlayer_;

        body = sf::RectangleShape(paddleSize);
        float newX = isPlayer ? paddleSize.x : windowX - paddleSize.x * 2;
        body.setPosition(newX, windowY / 2);
        
        body.setFillColor(isPlayer ? playerPaddleColor : enemyPaddleColor);
    }

    bool setPosition(float targetY) {
        if (targetY > windowY - paddleSize.y || targetY < 0)
            return false;
        body.setPosition(body.getPosition().x, targetY);
        return true;
    }
};

std::pair<bool, bool> circleRect(sf::Vector2f circlePos, float radius, sf::Vector2f rectPos, sf::Vector2f rectDimensions) {
    bool edgeHit = false;
    float testX = circlePos.x;
    float testY = circlePos.y;

    if (circlePos.x < rectPos.x)         testX = rectPos.x;      // left edge
    else if (circlePos.x > rectPos.x + rectDimensions.x) testX = rectPos.x + rectDimensions.x;   // right edge
    if (circlePos.y < rectPos.y)         testY = rectPos.y;      // top edge
    else if (circlePos.y > rectPos.y + rectDimensions.y) testY = rectPos.y + rectDimensions.y; // bottom edge

    if (testX == circlePos.x)
        edgeHit = true;
    float distX = circlePos.x - testX;
    float distY = circlePos.y - testY;
    float distance = (distX * distX) + (distY * distY);

    if (distance <= radius * radius) {
        return { true, edgeHit };
    }
    return { false, false };
}



int main() {
    sf::RenderWindow window(sf::VideoMode(windowX, windowY), "Pong");
    window.setFramerateLimit(240);

    sf::CircleShape debugCircle(2);
    debugCircle.setFillColor(sf::Color::Cyan);


    sf::Clock gameClock;
    gameClock.restart();

    sf::RectangleShape topWall(sf::Vector2f(1750, 70));
    topWall.setPosition((windowX - 1750) / 2, 50);
    topWall.setFillColor(sf::Color::Black);
    sf::RectangleShape bottomWall(sf::Vector2f(1750, 70));
    bottomWall.setPosition((windowX - 1750) / 2, windowY - 120);
    bottomWall.setFillColor(sf::Color::Black);

    Ball ball;
    Paddle player(true);
    Paddle enemy(false);

    int currentPlayerScore = 0;
    int currentEnemyScore = 0;
    float freezeTimer = 2.0f;
    float paddleBounceTimer = 0;
    float wallsBounceTimer = 0;

    EnemyOnStart();

    player.setPosition(sf::Mouse::getPosition().y - paddleSize.y * 1.5f);
    enemy.setPosition(ball.body.getPosition().y);

    window.clear(backgroundColor);
    window.draw(topWall);
    window.draw(bottomWall);
    window.draw(ball.body);
    window.draw(player.body);
    window.draw(enemy.body);
    window.display();

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }
        
        float deltaTime = gameClock.restart().asSeconds();
        if (freezeTimer > 0) {
            freezeTimer -= deltaTime;
            continue;
        }

        if (paddleBounceTimer > 0)
            paddleBounceTimer -= deltaTime;

        if (wallsBounceTimer > 0)
            wallsBounceTimer -= deltaTime;
        
        window.clear(backgroundColor);

        player.setPosition(sf::Mouse::getPosition().y - paddleSize.y * 1.5f);
        
        
        
        ball.body.setPosition(ball.body.getPosition() + ball.speed * deltaTime);
        const sf::Vector2f currentBallPos = ball.body.getPosition() + sf::Vector2f(ballSize, ballSize);
        
        
       
        bool debug = enemy.setPosition(EnemyUpdate(enemy.body.getPosition().x - currentBallPos.x, currentBallPos.y, ball.speed.x, ball.speed.y, deltaTime) - paddleSize.y / 2.0f);

        std::pair<bool, bool> playerHit = circleRect(currentBallPos, ballSize, player.body.getPosition(), paddleSize);
        std::pair<bool, bool> enemyHit = circleRect(currentBallPos, ballSize, enemy.body.getPosition(), paddleSize);

        if (paddleBounceTimer <= 0 && (playerHit.first || enemyHit.first))
        {
            if (playerHit.second || enemyHit.second)
                ball.speed.y *= -1;
            else
                ball.speed.x *= -1;

            paddleBounceTimer = 0.5f;
        }

        std::pair<bool, bool> topWallHit = circleRect(currentBallPos, ballSize, topWall.getPosition(), topWall.getSize());
        std::pair<bool, bool> bottomWallHit = circleRect(currentBallPos, ballSize, bottomWall.getPosition(), bottomWall.getSize());

        if (wallsBounceTimer <= 0 && topWallHit.first || bottomWallHit.first) {
            ball.speed.y *= -1;
            wallsBounceTimer = 0.5f;
        }

        bool playerHasScored = currentBallPos.x < 0;

        if (playerHasScored || currentBallPos.x > windowX) {
            ball.body.setPosition(windowX / 2, windowY / 2);
            ball.speed = startingBallSpeed;
            currentPlayerScore += playerHasScored;
            currentEnemyScore += !playerHasScored;
            EnemyOnScore(currentPlayerScore, currentEnemyScore);

            freezeTimer = 2.0f;
        }

        window.draw(topWall);
        window.draw(bottomWall);
        window.draw(ball.body);
        window.draw(player.body);
        window.draw(enemy.body);
        window.draw(debugCircle);

        window.display();
    }
    return 0;
}
