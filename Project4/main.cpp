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
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Entity.h"
#define PLATFORM_COUNT 46
#define FIXED_TIMESTEP 0.0166666f

struct GameState {
    Entity* player;
    Entity* platforms;
    Entity* success;
    Entity* fail;
    Entity* enemies;
    int enemy_count=3;
};

GameState state;

SDL_Window* displayWindow;
bool gameIsRunning = true;
bool missionCompleted = false;
bool missionFailed = false;
GLuint fontTextureID;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;


void DrawText(ShaderProgram* program, GLuint fontTextureID, std::string text,
    float size, float spacing, glm::vec3 position)
{
    float width = 1.0f / 16.0f;
    float height = 1.0f / 16.0f;
    std::vector<float> vertices;
    std::vector<float> texCoords;
    for (int i = 0; i < text.size(); i++) {
        int index = (int)text[i];
        float offset = (size + spacing) * i;
        float u = (float)(index % 16) / 16.0f;
        float v = (float)(index / 16) / 16.0f;
        vertices.insert(vertices.end(), {
offset + (-0.5f * size), 0.5f * size,
offset + (-0.5f * size), -0.5f * size,
offset + (0.5f * size), 0.5f * size,
offset + (0.5f * size), -0.5f * size,
offset + (0.5f * size), 0.5f * size,
offset + (-0.5f * size), -0.5f * size,
            });
        texCoords.insert(texCoords.end(), {
        u, v,
        u, v + height,
        u + width, v,
        u + width, v + height,
        u + width, v,
        u, v + height,
            });
    } // end of for loop
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
    program->SetModelMatrix(modelMatrix);
    glUseProgram(program->programID);
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices.data());
    glEnableVertexAttribArray(program->positionAttribute);
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords.data());
    glEnableVertexAttribArray(program->texCoordAttribute);
    glBindTexture(GL_TEXTURE_2D, fontTextureID);
    glDrawArrays(GL_TRIANGLES, 0, (int)(text.size() * 6));
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}



GLuint LoadTexture(const char* filePath) {
    int w, h, n;
    unsigned char* image = stbi_load(filePath, &w, &h, &n, STBI_rgb_alpha);

    if (image == NULL) {
        std::cout << "Unable to load image. Make sure the path is correct\n";
        assert(false);
    }

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    stbi_image_free(image);
    return textureID;
}


void Initialize() {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Textured!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);

#ifdef _WINDOWS
    glewInit();
#endif

    glViewport(0, 0, 640, 480);

    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");

    viewMatrix = glm::mat4(1.0f);
    modelMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);

    glUseProgram(program.programID);

    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glEnable(GL_BLEND);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    // Initialize Game Objects

    // Initialize Player
    state.player = new Entity();
    state.player->entityType = PLAYER;
    state.player->acceleration = glm::vec3(0, -10.0f, 0);
    state.player->position = glm::vec3(-4.0f,-1.0f,0);
    state.player->movement = glm::vec3(0);
    state.player->speed = 1.0f;
    state.player->textureID = LoadTexture("player.png");
    fontTextureID = LoadTexture("font1.png");
    state.player->width = 0.75f;
    state.player->height = 0.9;
    state.player->jumpPower = 8.0f;

    state.platforms = new Entity[PLATFORM_COUNT];
    state.enemies = new Entity[state.enemy_count];
    GLuint enemyTextureID = LoadTexture("ctg1.png");

    for (int i = 0; i < state.enemy_count; i++) {
        state.enemies[i].entityType = ENEMY;
        state.enemies[i].textureID = enemyTextureID;

        //bottom enemy
        if (i == 0) {
            state.enemies[0].position = glm::vec3(4, -2.35f, 0);
            state.enemies[0].aiType = WAITANDGO;
            state.enemies[0].aiState = IDLE;
            state.enemies[0].height = 0.5f;
            state.enemies[0].width = 0.5f;
        }

        //middle tier enemy
        if (i == 1) {
            state.enemies[1].position = glm::vec3(-0.5, 0.24f, 0);
            state.enemies[1].aiType = WALKER;
            state.enemies[1].aiState = PATROLLING;
            state.enemies[1].patrolMin = -1.21f;
            state.enemies[1].patrolMax = 1.51f;
            state.enemies[1].height = 0.5f;
            state.enemies[1].width = 0.5f;
        }

        //top enemy
        if (i == 2) {
            state.enemies[2].position = glm::vec3(-3.5f, 1.7f, 0);
            state.enemies[2].aiType = JUMPER;
            //state.enemies[2].jump = true;
            state.enemies[2].aiState = JUMPING;
            state.enemies[2].patrolMax = -0.5f;
            state.enemies[2].patrolMin = -3.5f;
            state.enemies[2].height = 0.5f;
            state.enemies[2].width = 0.5f;
            state.enemies[2].velocity.y += 1.0f;
            state.enemies[2].acceleration = glm::vec3(0, -2.0f, 0.0f);
            //movement.x = 1.0f;
            state.enemies[2].speed = 1.0f;
        }
        state.enemies[i].speed = 1.0f;
    }
    

    GLuint platformTextureID = LoadTexture("platformPack_tile001.png");

    float x = -4.5f;
    //floor tiles
    for (int i = 0; i < 10; i++) {
        state.platforms[i].entityType = PLATFORM;
        state.platforms[i].textureID = platformTextureID;
        state.platforms[i].position = glm::vec3(x, -3.25f, 0);
        x = x + 1.0f;
    }
    x = -3.5f;
    for (int i = 10; i < 13; i++) {
        state.platforms[i].entityType = PLATFORM;
        state.platforms[i].textureID = platformTextureID;
        state.platforms[i].position = glm::vec3(x, 1.0f, 0);
        x = x + 1.0f;
    }
    x = -1.0f;
    for (int i = 13; i < 16; i++) {
        state.platforms[i].entityType = PLATFORM;
        state.platforms[i].textureID = platformTextureID;
        state.platforms[i].position = glm::vec3(x, -0.5f, 0);
        x = x + 1.0f;
    }
    
    //left wall tiles
    float y=-4.25f;
    for (int i = 16; i < 26; i++) {
        state.platforms[i].entityType = PLATFORM;
        state.platforms[i].textureID = platformTextureID;
        state.platforms[i].position = glm::vec3(-5.0f, y, 0);
        y = y + 1.0f;
    }
    y = -4.25f;
    //right wall tiles
    for (int i = 26; i < 36; i++) {
        state.platforms[i].entityType = PLATFORM;
        state.platforms[i].textureID = platformTextureID;
        state.platforms[i].position = glm::vec3(5.0f, y, 0);
        y = y + 1.0f;
    }
    
    //top tiles
    x = -4.5f;
    for (int i = 36; i < 46; i++) {
        state.platforms[i].entityType = PLATFORM;
        state.platforms[i].textureID = platformTextureID;
        state.platforms[i].position = glm::vec3(x, 4.0f, 0);
        x = x + 1.0f;
    }
    
    
    for (int i = 0; i < PLATFORM_COUNT; i++) {
        state.platforms[i].Update(0, NULL, NULL, 0);
    }
    for (int i = 0; i < state.enemy_count; i++) {
        state.enemies[i].Update(FIXED_TIMESTEP, state.player, state.platforms, PLATFORM_COUNT);
    }
    

}

void ProcessInput() {

    state.player->movement = glm::vec3(0);

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
        case SDL_WINDOWEVENT_CLOSE:
            gameIsRunning = false;
            break;

        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
            case SDLK_LEFT:
                if ((state.player->position.x - 0.1 > -4.0f)) {
                    state.player->position.x -= 0.1f;
                }
                break;

            case SDLK_RIGHT:
                if (state.player->position.x - 0.1 < 4.0f) {
                    state.player->position.x += 0.1f;
                }
                break;

            case SDLK_SPACE:
                if (state.player->collidedBottom) {
                    state.player->jump = true;
                }
                break;
            }
            break; // SDL_KEYDOWN
        }
    }

    const Uint8* keys = SDL_GetKeyboardState(NULL);

    if (keys[SDL_SCANCODE_LEFT]) {
        if (!missionCompleted && !missionFailed) {
            state.player->movement.x = -1.0f;
        }
    }
    else if (keys[SDL_SCANCODE_RIGHT]) {
        if (!missionCompleted && !missionFailed) {
            state.player->movement.x = 1.0f;
        }
    }
    //else if (keys[SDL_SCANCODE_SPACE]) {
    //    state.player->jump = true;
    //}


    if (glm::length(state.player->movement) > 1.0f) {
        state.player->movement = glm::normalize(state.player->movement);
    }

}


float lastTicks = 0;
float accumulator = 0.0f;
void Update() {
    if (missionCompleted || missionFailed) { return; }
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;

    deltaTime += accumulator;
    if (deltaTime < FIXED_TIMESTEP) {
        accumulator = deltaTime;
        return;
    }
    state.player->CheckCollisionsY(state.enemies, state.enemy_count);
    state.player->CheckCollisionsX(state.enemies, state.enemy_count);
    if (state.player->collidedEnemy != NULL) {
        std::cout << "running?" << std::endl;
        for (int i = 0; i < state.enemy_count; i++) {
            std::cout << state.player->hitEnemy << std::endl;
            std::cout << state.player->killEnemy << std::endl;
            std::cout << state.enemies[i].position.y << std::endl;
            if (state.player->collidedEnemy == &state.enemies[i] && (state.player->hitEnemy == true)) {
                missionFailed = true;
                std::cout << "failing?" << std::endl;
                return;
            }
            else if (state.player->collidedEnemy == &state.enemies[i] && (state.player->killEnemy== true)) {
                std::cout << "hit enemy?" << std::endl;
                state.player->killEnemy == false;
                state.enemies[i].position = glm::vec3(10.0f, 10.0f, 0.0);
                Entity* new_enemies = new Entity[state.enemy_count - 1];
                int l=0;
                for (int k = 0; k < state.enemy_count-1; k++) {
                    if (&state.enemies[l] == &state.enemies[i]) {
                        l++;
                    }
                    new_enemies[k] = state.enemies[l];
                    
                    l++;
                }
                state.enemy_count -=1;
                delete[] state.enemies;
                state.enemies = new_enemies;

               
                if (state.enemy_count == 0) {
                    missionCompleted = true;
                    return;
                }
                state.player->collidedEnemy = NULL;
                break;
            }

        }
        
    }
    while (deltaTime >= FIXED_TIMESTEP) {
        // Update. Notice it's FIXED_TIMESTEP. Not deltaTime
        state.player->Update(FIXED_TIMESTEP,state.player, state.platforms, PLATFORM_COUNT);
        for (int i = 0; i < state.enemy_count; i++) {
            state.enemies[i].Update(FIXED_TIMESTEP, state.player, state.platforms, PLATFORM_COUNT);
        }
        deltaTime -= FIXED_TIMESTEP;
    }
    accumulator = deltaTime;

}

void Render() {
    glClear(GL_COLOR_BUFFER_BIT);

    for (int i = 0; i < PLATFORM_COUNT; i++) {
        state.platforms[i].Render(&program);
    }
    for (int i = 0; i < state.enemy_count; i++) {
        state.enemies[i].Render(&program);
    }
    state.player->Render(&program);
    if (missionCompleted) {
        DrawText(&program, fontTextureID, "Mission Completed", 0.5f, -0.25f,
            glm::vec3(-4.75f, 3.3, 0));
    }
    if (missionFailed) {
        DrawText(&program, fontTextureID, "Mission Failed", 0.5f, -0.25f,
            glm::vec3(-4.75f, 3.3, 0));
    }

    SDL_GL_SwapWindow(displayWindow);
}


void Shutdown() {
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    Initialize();

    while (gameIsRunning) {
        ProcessInput();
        Update();
        Render();
    }

    Shutdown();
    return 0;
}