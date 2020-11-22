#include "Level2.h"
#define LEVEL2_WIDTH 17
#define LEVEL2_HEIGHT 8


unsigned int level2_data[] =
{
3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 
3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
3, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 3,
3, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 3,
3, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 2, 2, 3,
3, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 3
};

void Level2::Initialize() {
    //GLuint mapTextureID = Util::LoadTexture("tileset.png");
    state.nextScene = -1;
    GLuint mapTextureID = Util::LoadTexture("platformPack_tile001.png");
    state.map = new Map(LEVEL2_WIDTH, LEVEL2_HEIGHT, level2_data, mapTextureID, 1.0f, 4, 1);
    // Move over all of the player and enemy code from initialization.

        // Initialize Game Objects
    // Initialize Player
    state.player = new Entity();
    state.player->entityType = PLAYER;
    state.player->acceleration = glm::vec3(0.0f, -10.0f, 0);
    state.player->position = glm::vec3(5.0f, 0.0f, 0);
    state.player->movement = glm::vec3(0);
    state.player->speed = 2.0f;
    state.player->textureID = Util::LoadTexture("player.png");
    state.map = new Map(LEVEL2_WIDTH, LEVEL2_HEIGHT, level2_data, mapTextureID, 1.0f, 4, 1);
    state.player->width = 0.75f;
    state.player->height = 0.9;
    state.player->jumpPower = 5.0f;
    state.enemy_count = 3;

    //state.platforms = new Entity[PLATFORM_COUNT];
    state.enemies = new Entity[state.enemy_count];
    GLuint enemyTextureID = Util::LoadTexture("ctg1.png");
    GLuint doorTextureID = Util::LoadTexture("playerShip2_green.png");
    state.door = new Entity();
    state.door->textureID = doorTextureID;
    state.door->position = glm::vec3(13.0f, -4.0f, 0);
    state.door->entityType = PLATFORM;
    state.door->movement = glm::vec3(0.0f, 0.0f, 0);;
    state.door->acceleration = glm::vec3(0.0f, 0.0f, 0);;
    state.door->velocity = glm::vec3(0.0f, 0.0f, 0);

    for (int i = 0; i < state.enemy_count; i++) {
        state.enemies[i].entityType = ENEMY;
        state.enemies[i].textureID = enemyTextureID;
        state.enemies[i].aiType = WAITANDGO;
        state.enemies[i].aiState = IDLE;
        state.enemies[i].height = 0.5f;
        state.enemies[i].width = 0.5f;

        //bottom enemy
        if (i == 0) {
            state.enemies[i].position = glm::vec3(1.0f, -4.1f, 0);
        }

        //middle tier enemy
        if (i == 1) {
            state.enemies[i].position = glm::vec3(3.0f, -6.1f, 0);
        }
        if (i == 2) {
            state.enemies[i].position = glm::vec3(10.0f, -4.1f, 0);
            
        }


        state.enemies[i].speed = 1.0f;
        state.enemies[i].isActive = true;
    }


}
void Level2::Update(float deltaTime) {
    state.player->Update(deltaTime, state.player, state.map, state.enemies, state.enemy_count);
    state.door->Update(deltaTime, state.player, state.map, state.enemies, state.enemy_count);
    for (int i = 0; i < state.enemy_count; i++) {
        if (state.enemies[i].isActive) {
            state.enemies[i].Update(deltaTime, state.player, state.map, state.enemies, state.enemy_count);
        }
    }
    if (state.player->CheckCollision(state.door) && state.enemy_count == 0) {
        state.nextScene = 3;
    }
}
void Level2::Render(ShaderProgram* program) {
    state.map->Render(program);
    state.door->Render(program);
    state.player->Render(program);
    for (int i = 0; i < state.enemy_count; i++) {
        state.enemies[i].Render(program);
        //std::cout << "enemy "<<i<< " "<< state.enemies[i].position.x << " "<< state.enemies[i].position.y<< std::endl;
    }
    
}