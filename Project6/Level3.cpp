#include "Level3.h"
#define LEVEL3_WIDTH 40
#define LEVEL3_HEIGHT 8

unsigned int level3_data[] =
{
3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 3,
3, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 1, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 3,
3, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 0, 0, 3,
3, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 0, 3,
3, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 3,
3, 0, 0, 0, 0, 0, 0, 1, 1, 2, 2, 2, 2, 2, 2, 2, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
3, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3,
};

void Level3::Initialize() {
    //GLuint mapTextureID = Util::LoadTexture("tileset.png");
    state.nextScene = -1;
    GLuint mapTextureID = Util::LoadTexture("platformPack_tile001.png");
    state.map = new Map(LEVEL3_WIDTH, LEVEL3_HEIGHT, level3_data, mapTextureID, 1.0f, 4, 1);
    // Move over all of the player and enemy code from initialization.

        // Initialize Game Objects
    // Initialize Player
    state.player = new Entity();
    state.player->entityType = PLAYER;
    state.player->acceleration = glm::vec3(0.0f, 0.0f, 0);
    state.player->position = glm::vec3(8.0f, -1.1f, 0);
    state.player->movement = glm::vec3(0);
    state.player->speed = 2.0f;
    state.player->textureID = Util::LoadTexture("player.png");
    state.map = new Map(LEVEL3_WIDTH, LEVEL3_HEIGHT, level3_data, mapTextureID, 1.0f, 4, 1);
    state.player->width = 0.75f;
    state.player->height = 0.9;
    state.player->jumpPower = 5.0f;
    state.enemy_count = 16;

    //state.platforms = new Entity[PLATFORM_COUNT];
    state.enemies = new Entity[state.enemy_count];
    GLuint enemyTextureID = Util::LoadTexture("ctg1.png");
    GLuint doorTextureID = Util::LoadTexture("playerShip2_green.png");
    state.door = new Entity();
    state.door->textureID = doorTextureID;
    state.door->position = glm::vec3(35.0f, -1.1f, 0);
    state.door->entityType = PLATFORM;
    state.door->movement = glm::vec3(0.0f, 0.0f, 0);;
    state.door->acceleration = glm::vec3(0.0f, 0.0f, 0);;
    state.door->velocity = glm::vec3(0.0f, 0.0f, 0);

    for (int i = 0; i < state.enemy_count; i++) {
        state.enemies[i].entityType = ENEMY;
        state.enemies[i].textureID = enemyTextureID;
        state.enemies[i].height = 0.5f;
        state.enemies[i].width = 0.5f;

        //bottom enemy
        if (i == 0) {
            state.enemies[i].position = glm::vec3(1.0f, -3.1f, 0);
            state.enemies[i].aiType = WAITANDGO;
            state.enemies[i].aiState = IDLE;
        }

        //middle tier enemy
        if (i == 1) {
            state.enemies[i].position = glm::vec3(3.0f, -6.1f, 0);
            state.enemies[i].aiType = WAITANDGO;
            state.enemies[i].aiState = IDLE;
        }
        if (i == 2) {
            state.enemies[i].position = glm::vec3(10.0f, -4.1f, 0);
            state.enemies[i].aiType = WAITANDGO;
            state.enemies[i].aiState = IDLE;
        }

        if (i == 3) {
            state.enemies[i].position = glm::vec3(4.5f, -2.1f, 0);
            state.enemies[i].aiType = WAITANDGO;
            state.enemies[i].aiState = IDLE;

        }
        if (i == 4) {
            state.enemies[i].position = glm::vec3(8.0f, -5.1f, 0);
            state.enemies[i].aiType = WAITANDGO;
            state.enemies[i].aiState = IDLE;
        }
        if (i == 5) {
            state.enemies[i].position = glm::vec3(13.3f, -1.1f, 0);
            state.enemies[i].aiType = WAITANDGO;
            state.enemies[i].aiState = IDLE;
        }
        if (i == 6) {
            state.enemies[i].position = glm::vec3(13.3f, -3.1f, 0);
            state.enemies[i].aiType = WALKER;
            state.enemies[i].aiState = IDLE;
            state.enemies[i].patrolMax = 14.8f;
            state.enemies[i].patrolMin = 13.1f;
        }
        if (i == 7) {
            state.enemies[i].position = glm::vec3(18.0f, -6.1f, 0);
            state.enemies[i].aiType = WAITANDGO;
            state.enemies[i].aiState = IDLE;
        }
        if (i == 8) {
            state.enemies[i].position = glm::vec3(21.0f, -5.1f, 0);
            state.enemies[i].aiType = WAITANDGO;
            state.enemies[i].aiState = IDLE;
        }
        if (i == 9) {
            state.enemies[i].position = glm::vec3(18.0f, -1.1f, 0);
            state.enemies[i].aiType = WAITANDGO;
            state.enemies[i].aiState = IDLE;
        }
        if (i == 10) {
            state.enemies[i].position = glm::vec3(16.0f, -3.1f, 0);
            state.enemies[i].aiType = WALKER;
            state.enemies[i].aiState = IDLE;
            state.enemies[i].patrolMax = 18.0f;
            state.enemies[i].patrolMin = 15.0f;
        }
        if (i == 11) {
            state.enemies[i].position = glm::vec3(21.0f, -3.1f, 0);
            state.enemies[i].aiType = WALKER;
            state.enemies[i].aiState = IDLE;
            state.enemies[i].patrolMax = 22.0f;
            state.enemies[i].patrolMin = 20.1f;
        }
        if (i == 12) {
            state.enemies[i].position = glm::vec3(26.0f, -1.1f, 0);
            state.enemies[i].aiType = WALKER;
            state.enemies[i].aiState = IDLE;
            state.enemies[i].patrolMax = 29.0f;
            state.enemies[i].patrolMin = 25.0f;
        }
        if (i == 13) {
            state.enemies[i].position = glm::vec3(27.0f, -4.1f, 0);
            state.enemies[i].aiType = WALKER;
            state.enemies[i].aiState = IDLE;
            state.enemies[i].patrolMax = 29.0f;
            state.enemies[i].patrolMin = 25.0f;
        }
        if (i == 14) {
            state.enemies[i].position = glm::vec3(34.0f, -5.1f, 0);
            state.enemies[i].aiType = WALKER;
            state.enemies[i].aiState = IDLE;
            state.enemies[i].patrolMax = 35.0f;
            state.enemies[i].patrolMin = 32.0f;
        }
        if (i == 15) {
            state.enemies[i].position = glm::vec3(36.0f, -2.1f, 0);
            state.enemies[i].aiType = WALKER;
            state.enemies[i].aiState = IDLE;
            state.enemies[i].patrolMax = 37.9f;
            state.enemies[i].patrolMin = 35.1f;
        }
        state.enemies[i].speed = 1.0f;
        state.enemies[i].isActive = true;
    }
    GLuint coinTextureID = Util::LoadTexture("coin.png");
    state.coin_count = 11;
    state.coins = new Entity[state.coin_count];
    for (int i = 0; i < state.coin_count; i++) {
        state.coins[i].textureID = coinTextureID;

        state.coins[i].entityType = COIN;
        state.coins[i].movement = glm::vec3(0.0f, 0.0f, 0);;
        state.coins[i].acceleration = glm::vec3(0.0f, 0.0f, 0);
        state.coins[i].velocity = glm::vec3(0.0f, 0.0f, 0);
        state.coins[i].speed = 0.0f;
        if (i == 0) {
            state.coins[i].position = glm::vec3(3.2f, -2.0f, 0);
        }
        else if (i == 1) {
            state.coins[i].position = glm::vec3(2.0f, -3.1f, 0);
        }
        else if (i == 2) {
            state.coins[i].position = glm::vec3(5.0f, -6.1f, 0);
        }
        else if (i == 3) {
            state.coins[i].position = glm::vec3(13.0f, -4.1f, 0);
        }
        else if (i == 4) {
            state.coins[i].position = glm::vec3(10.0f, -1.1f, 0);
        }
        else if (i == 5) {
            state.coins[i].position = glm::vec3(13.0f, -2.1f, 0);
        }
        else if (i == 6) {
            state.coins[i].position = glm::vec3(16.0f, -2.1f, 0);
        }
        else if (i == 7) {
            state.coins[i].position = glm::vec3(19.0f, -1.1f, 0);
        }
        else if (i == 8) {
            state.coins[i].position = glm::vec3(22.0f, -3.1f, 0);
        }
        else if (i == 9) {
            state.coins[i].position = glm::vec3(26.0f, -1.1f, 0);
        }
        else if (i==10){
            state.coins[i].position = glm::vec3(33.0f, -1.1f, 0);
        }
        else {
            state.coins[i].position = glm::vec3(25.0f, -3.1f, 0);
        }
    }

}
void Level3::Update(float deltaTime) {
    state.player->Update(deltaTime, state.player, state.map, state.enemies, state.enemy_count, state.coins, state.coin_count);
    state.door->Update(deltaTime, state.player, state.map, state.enemies, state.enemy_count);
    for (int i = 0; i < state.enemy_count; i++) {
        if (state.enemies[i].isActive) {
            state.enemies[i].Update(deltaTime, state.player, state.map, state.enemies, state.enemy_count);
        }
    }
    for (int i = 0; i < state.coin_count; i++) {
        state.coins[i].Update(deltaTime, state.player, state.map, state.enemies, state.enemy_count);
    }
    if (state.player->CheckCollision(state.door) && state.coin_count == 0) {
        state.nextScene = 0;
    }
}
void Level3::Render(ShaderProgram* program) {
    state.map->Render(program);
    state.door->Render(program);
    state.player->Render(program);
    
    for (int i = 0; i < state.enemy_count; i++) {
        state.enemies[i].Render(program);
        //std::cout << "enemy "<<i<< " "<< state.enemies[i].position.x << " "<< state.enemies[i].position.y<< std::endl;
    }
    for (int i = 0; i < state.coin_count; i++) {
        state.coins[i].Render(program);
        //std::cout << "enemy "<<i<< " "<< state.enemies[i].position.x << " "<< state.enemies[i].position.y<< std::endl;

    }
    
}