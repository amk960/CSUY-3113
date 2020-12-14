#pragma once
#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "Map.h"

enum EntityType {PLAYER, PLATFORM,COIN, ENEMY};
enum AIType {WALKER, WAITANDGO, JUMPER};
enum AIState {IDLE, WALKING, JUMPING, PATROLLING};
class Entity {
public:
    AIType aiType;
    EntityType entityType;
    AIState aiState;
    glm::vec3 position;
    glm::vec3 movement;
    glm::vec3 acceleration;
    glm::vec3 velocity;
    float patrolDir = 1.0f;
    float speed;
    float width = 1;
    float height = 1;
    float rotate = 0.0f;
    bool jump = false;
    float jumpPower = 0;
    float patrolMax;
    float patrolMin;
    GLuint textureID;

    glm::mat4 modelMatrix;

    int* animRight = NULL;
    int* animLeft = NULL;
    int* animUp = NULL;
    int* animDown = NULL;

    int* animIndices = NULL;
    int animFrames = 0;
    int animIndex = 0;
    float animTime = 0;
    int animCols = 0;
    int animRows = 0;
    bool isActive = true;
    bool collidedTop = false;
    bool collidedBottom = false;
    bool collidedLeft = false;
    bool collidedRight = false;
    //Entity* collidedWith = NULL;
    Entity* collidedEnemy = NULL;
    Entity* collidedCoin = NULL;
    bool hitEnemy = false;
    bool killEnemy = false;
    Entity();

    bool CheckCollision(Entity* Other);
    void Update(float deltaTime,Entity* player, Map*map, Entity* objects1, int objectCount1, Entity* objects2 = NULL, int objectCount2 = NULL);
    void Render(ShaderProgram* program);
    void DrawSpriteFromTextureAtlas(ShaderProgram* program, GLuint textureID, int index);
    void CheckCollisionsY(Entity* objects, int objectCount);
    void CheckCollisionsX(Entity* objects, int objectCount);
    void AI(Entity* player, Map* map);
    void AIWalker();
    void AIJumper();
    void AIWaitAndGo(Entity* player);
    void CheckCollisionsX(Map* map);
    void CheckCollisionsY(Map* map);
};
