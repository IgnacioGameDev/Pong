#include <iostream>

int enemyPosY;
float enemySpeedY;
float enemyAcceleration;

float visibilityDistance; // Enemy is blind beyond visibilityDistance and will just move towards the center
float reactionDistance; // The distance at which the enemy will start decelerating when it's approaching it's target


const int windowX = 1920;
const int windowY = 1080;
const float wallMargin = 210;
const float topSpeed = 30;
const float dampSpeed = 0.8f;
const float dampAcceleration = 0.3f;


float clamp(float num, float min, float max) {
    return std::max(min, std::min(num, max));
}

void EnemyOnStart() {
    enemyPosY = windowY * 0.5f;
    enemySpeedY = 0;
    enemyAcceleration = 7;
    visibilityDistance = windowX * 0.75f;
    reactionDistance = 200;
}

// An enemy 10 points behind is almost unbeatable, an enemy 10 points ahead will fail to reach any less than easy ball
void EnemyOnScore(int playerScore, int enemyScore) {
    enemyPosY = windowY * 0.5f;
    enemySpeedY = 0;
    visibilityDistance += playerScore > enemyScore ? windowX * 0.04f : -windowX * 0.04f;
    
    visibilityDistance = clamp(visibilityDistance, windowX * 0.5f, windowX);
    reactionDistance += playerScore > enemyScore ? 10 : -10;
    reactionDistance = clamp(reactionDistance, 50, 300);
}

//"ballDistX" is input as enemyPosX - ballPosX because enemy is on the right side and has an offset away from the side of the screen
float EnemyUpdate(float ballDistX, float ballPosY, float ballSpeedX, float ballSpeedY, float dt) {
    float timeX = ballDistX / std::abs(ballSpeedX);
    float targetY = ballPosY + ballSpeedY * timeX;
    targetY = clamp(targetY, wallMargin, windowY - wallMargin);

    if (ballDistX > visibilityDistance || ballSpeedX < 0)
        targetY = windowY * 0.5f;

    float deltaY = targetY - enemyPosY;
    int dir = deltaY > 0 ? 1 : -1;

    if (enemySpeedY / std::abs(enemySpeedY) != dir && std::abs(deltaY) < reactionDistance) {
        enemySpeedY *= dampSpeed;
        enemySpeedY -= enemyAcceleration * dampAcceleration * dir * dt;
    }
    else {
        enemySpeedY += enemyAcceleration * dir * dt;
    }

    enemySpeedY = clamp(enemySpeedY, -topSpeed, topSpeed);
    enemyPosY += enemySpeedY;
    return enemyPosY;
}