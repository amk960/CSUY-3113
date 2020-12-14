#include "Level1.h"
#define LEVEL1_WIDTH 20
#define LEVEL1_HEIGHT 8


unsigned int level1_data[] =
{
3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 
3, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 3,
3, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 3,
3, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 3,
3, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
3, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 3,
3, 1, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 3,
3, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3
};

void Level1::Initialize() {
	//GLuint mapTextureID = Util::LoadTexture("tileset.png");
    state.nextScene = -1;
    GLuint mapTextureID = Util::LoadTexture("platformPack_tile001.png");
    GLuint coinTextureID = Util::LoadTexture("coin.png");
	state.map = new Map(LEVEL1_WIDTH, LEVEL1_HEIGHT, level1_data, mapTextureID, 1.0f, 4, 1);
	// Move over all of the player and enemy code from initialization.

        // Initialize Game Objects
    // Initialize Player
    state.player = new Entity();
    state.player->entityType = PLAYER;
    state.player->acceleration = glm::vec3(0.0f, 0.0f, 0);
    state.player->position = glm::vec3(2.0f, -1.0f, 0);
    state.player->movement = glm::vec3(0);
    state.player->speed = 2.0f;
    state.player->textureID = Util::LoadTexture("player.png");
    state.map = new Map(LEVEL1_WIDTH, LEVEL1_HEIGHT, level1_data, mapTextureID, 1.0f, 4, 1);
    state.player->width = 0.8f;
    state.player->height = 0.8f;
    
    

    //state.platforms = new Entity[PLATFORM_COUNT];
    
    GLuint enemyTextureID = Util::LoadTexture("ctg1.png");
    GLuint doorTextureID = Util::LoadTexture("playerShip2_green.png");
    state.door = new Entity();
    state.door->textureID = doorTextureID;
    state.door->position = glm::vec3(18.0f, -5.0f, 0);
    state.door->entityType = PLATFORM;
    state.door->movement = glm::vec3(0.0f, 0.0f, 0);;
    state.door->acceleration = glm::vec3(0.0f, 0.0f, 0);
    state.door->velocity= glm::vec3(0.0f, 0.0f, 0);

    state.enemy_count = 4;
    state.enemies = new Entity[state.enemy_count];
    for (int i = 0; i < state.enemy_count; i++) {
        state.enemies[i].entityType = ENEMY;
        state.enemies[i].textureID = enemyTextureID;
        state.enemies[i].aiType = WALKER;
        state.enemies[i].aiState = IDLE;
        state.enemies[i].height = 0.5f;
        state.enemies[i].width = 0.5f;

        //bottom enemy
        if (i == 0) {
            state.enemies[i].position = glm::vec3(3.0f, -6.0f, 0);
            state.enemies[i].patrolMax = 4.0f;
            state.enemies[i].patrolMin = 2.5f;
        }

        //middle tier enemy
        if (i == 1) {
            state.enemies[i].position = glm::vec3(6.0f, -5.1f, 0);
            state.enemies[i].patrolMax = 7.0f;
            state.enemies[i].patrolMin = 5.7f;

        }

        if (i == 2) {
            state.enemies[i].position = glm::vec3(12.0f, -1.1f, 0);
            state.enemies[i].patrolMax = 13.0f;
            state.enemies[i].patrolMin = 11.0f;

        }
        if (i == 3) {
            state.enemies[i].position = glm::vec3(17.0f, -2.1f, 0);
            state.enemies[i].patrolMax = 18.0f;
            state.enemies[i].patrolMin = 16.0f;

        }

        state.enemies[i].speed = 1.0f;
        state.enemies[i].isActive = true;
    }

    state.coin_count = 5;
    state.coins = new Entity[state.coin_count];
    for (int i = 0; i < state.coin_count; i++) {
        state.coins[i].entityType = PLATFORM;
        state.coins[i].textureID = coinTextureID;
        
        state.coins[i].entityType = COIN;
        state.coins[i].movement = glm::vec3(0.0f, 0.0f, 0);;
        state.coins[i].acceleration = glm::vec3(0.0f, 0.0f, 0);
        state.coins[i].velocity = glm::vec3(0.0f, 0.0f, 0);
        state.coins[i].speed = 0.0f;
        if (i == 0) {
            state.coins[i].position = glm::vec3(4.2f, -1.5f, 0);
        }
        else if (i == 1) {
            state.coins[i].position = glm::vec3(1.9f, -6.0f, 0);
        }
        else if (i == 2) {
            state.coins[i].position = glm::vec3(5.0f, -5.1f, 0);
        }
        else if (i == 3) {
            state.coins[i].position = glm::vec3(16.0f, -1.1f, 0);
        }
        else {
            state.coins[i].position = glm::vec3(10.0f, -1.1f, 0);
        }

    }
}
void Level1::Update(float deltaTime) {
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
    if (state.player->CheckCollision(state.door) && state.coin_count==0) {
        state.nextScene = 2;
    }

}
void Level1::Render(ShaderProgram* program) {
	state.map->Render(program);
    state.door->Render(program);
	state.player->Render(program);
    //std::cout << "player "<<state.player->position.x << " " << state.player->position.y << std::endl;
    for (int i = 0; i < state.enemy_count; i++) {
        state.enemies[i].Render(program);
        //std::cout << "enemy "<<i<< " "<< state.enemies[i].position.x << " "<< state.enemies[i].position.y<< std::endl;

    }
    for (int i = 0; i < state.coin_count; i++) {
        state.coins[i].Render(program);
        //std::cout << "enemy "<<i<< " "<< state.enemies[i].position.x << " "<< state.enemies[i].position.y<< std::endl;

    }

}