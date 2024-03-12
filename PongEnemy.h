#pragma once
void EnemyOnStart();
void EnemyOnScore(int playerScore, int enemyScore);
float EnemyUpdate(float ballPosX, float ballPosY, float ballSpeedX, float ballSpeedY, float dt);