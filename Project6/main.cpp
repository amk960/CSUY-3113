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
#include "Util.h"
#include <string>
#include <SDL_mixer.h>

#include "Entity.h" 
#include "Map.h"
#include "Scene.h"
#include "Level1.h"
#include "Level2.h"
#include "Level3.h"
#include "Menu.h"

#define FIXED_TIMESTEP 0.0166666f



SDL_Window* displayWindow;
bool gameIsRunning = true;
bool missionCompleted = false; 
bool missionFailed = false;
//string livesStr;
int lives = 3;
Mix_Music* music;
Mix_Chunk* bounce;
Mix_Chunk* kill;
Mix_Chunk* hit;
Mix_Chunk* gameover;

GLuint fontTextureID;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;


// Remove Level1 *level1 and add an array of scenes
Scene* currentScene;
Scene* sceneList[4];


void SwitchToScene(Scene* scene) {
    currentScene = scene;
    currentScene->Initialize();
}


void Initialize() {
    
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
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

    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    music = Mix_LoadMUS("Darkling.mp3");
    Mix_PlayMusic(music,-1);
    Mix_VolumeMusic(MIX_MAX_VOLUME / 4);
    //bounce = Mix_LoadWAV("bounce.wav");
    hit = Mix_LoadWAV("fail.wav");
    kill = Mix_LoadWAV("success.wav");
    gameover = Mix_LoadWAV("gameover.wav");

    fontTextureID = Util::LoadTexture("font1.png");


    // Initialize the levels and start at the first one
    sceneList[0] = new Menu();
    sceneList[1] = new Level1();
    sceneList[2] = new Level2();
    sceneList[3] = new Level3();
    SwitchToScene(sceneList[0]);
    
}

void ProcessInput() {

    currentScene->state.player->movement = glm::vec3(0);

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
                if ((currentScene->state.player->position.x - 0.1 > -4.0f)) {
                    currentScene->state.player->position.x -= 0.1f;
                    //currentScene->state.player->CheckCollisionsX(currentScene->state.map);
                }
                break;
            case SDLK_UP:
                if ((currentScene->state.player->position.y + 0.1 < -0.5f)) {
                    currentScene->state.player->position.y += 0.1f;
                    currentScene->state.player->CheckCollisionsY(currentScene->state.map);
                }
                break;
            case SDLK_DOWN:
                if ((currentScene->state.player->position.y - 0.1 > -7.0f)) {
                    currentScene->state.player->position.y -= 0.1f;
                    currentScene->state.player->CheckCollisionsY(currentScene->state.map);
                }
                break;

            case SDLK_RIGHT:
                if (currentScene->state.player->position.x - 0.1 < 4.0f) {
                    currentScene->state.player->position.x += 0.1f;
                    //currentScene->state.player->CheckCollisionsX(currentScene->state.map);
                }
                break;
            case SDLK_RETURN:
                if (currentScene==sceneList[0]) {
                    SwitchToScene(sceneList[1]);
                }
                break;
            //case SDLK_SPACE:
             //   if (currentScene->state.player->collidedBottom) {
             //       currentScene->state.player->jump = true;
             //       Mix_PlayChannel(-1, bounce, 0);
             //   }
             //   break;
            }
            break; // SDL_KEYDOWN
        }
    }

    const Uint8* keys = SDL_GetKeyboardState(NULL);

    if (keys[SDL_SCANCODE_LEFT]) {
        if (!missionCompleted && !missionFailed) {
            currentScene->state.player->movement.x = -1.0f;

        }
    }
    else if (keys[SDL_SCANCODE_RIGHT]) {
        if (!missionCompleted && !missionFailed) {
            currentScene->state.player->movement.x = 1.0f;
        }
    }
    else if (keys[SDL_SCANCODE_UP]) {
        if (!missionCompleted && !missionFailed) {
            currentScene->state.player->movement.y = 1.0f;
            currentScene->state.player->CheckCollisionsY(currentScene->state.map);
        }
    }
    else if (keys[SDL_SCANCODE_DOWN]) {
        if (!missionCompleted && !missionFailed) {
            currentScene->state.player->movement.y = -1.0f;
            currentScene->state.player->CheckCollisionsY(currentScene->state.map);
        }
    }
    //else if (keys[SDL_SCANCODE_SPACE]) {
    //    state.player->jump = true;
    //}


    if (glm::length(currentScene->state.player->movement) > 1.0f) {
        currentScene->state.player->movement = glm::normalize(currentScene->state.player->movement);
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


    currentScene->state.player->CheckCollisionsY(currentScene->state.enemies, currentScene->state.enemy_count);
    currentScene->state.player->CheckCollisionsX(currentScene->state.enemies, currentScene->state.enemy_count);
    if (currentScene->state.player->collidedEnemy != NULL) {
        std::cout << "WHAT" << std::endl;
        for (int i = 0; i < currentScene->state.enemy_count; i++) {
            std::cout << "each" << std::endl;
            if (currentScene->state.player->collidedEnemy == &(currentScene->state.enemies[i]) && (currentScene->state.player->hitEnemy == true) ) {
                std::cout << "WHATTTTT" << std::endl;
                Mix_PlayChannel(-1, hit, 0);
                lives = lives--;
                currentScene->state.player->hitEnemy = false;
                currentScene->state.player->killEnemy = false;
                currentScene->state.player->collidedEnemy = NULL;
                if (lives == 0) { missionFailed = true; Mix_PlayChannel(-1, gameover, 0); return; }
                for (int i = 0; i < 4; i++) {
                    if (sceneList[i] == currentScene) {
                        currentScene->Initialize();
                        return;
                    }
                }
            }

        }
        
    }
    if (currentScene->state.player->collidedCoin != NULL) {
        for (int i = 0; i < currentScene->state.coin_count; i++) {
            if (currentScene->state.player->collidedCoin == &(currentScene->state.coins[i])) {
                currentScene->state.player->collidedCoin = NULL;
                Entity* new_coins = new Entity[currentScene->state.coin_count - 1];
                Mix_PlayChannel(-1, kill, 0);
                int l = 0;
                for (int k = 0; k < currentScene->state.coin_count - 1; k++) {
                    if (&(currentScene->state.coins[l]) == &(currentScene->state.coins[i])) {
                        l++;
                        //continue;
                    }
                    new_coins[k] = currentScene->state.coins[l];

                    l++;
                }
                currentScene->state.coin_count -= 1;
                delete[] currentScene->state.coins;
                currentScene->state.coins = new_coins;
                break;
            }
        }
    }
    
    
    while (deltaTime >= FIXED_TIMESTEP) {
        // Update. Notice it's FIXED_TIMESTEP. Not deltaTime
        currentScene->Update(FIXED_TIMESTEP);
        //for (int i = 0; i < state.enemy_count; i++) {
        //    state.enemies[i].Update(FIXED_TIMESTEP, state.player, state.map, );
        //}
        deltaTime -= FIXED_TIMESTEP;
    }
    
    accumulator = deltaTime;

    viewMatrix = glm::mat4(1.0f);
    viewMatrix = glm::translate(viewMatrix, glm::vec3(-currentScene->state.player->position.x, 3.75f, 0.0f));
    // Inside Update
    viewMatrix = glm::mat4(1.0f);
    if (currentScene->state.player->position.x > 5) {
        viewMatrix = glm::translate(viewMatrix,
            glm::vec3(-currentScene->state.player->position.x, 3.75, 0));
    }
    else {
        viewMatrix = glm::translate(viewMatrix, glm::vec3(-5, 3.75, 0));
    }

}

void Render() {
    glClear(GL_COLOR_BUFFER_BIT);
    program.SetViewMatrix(viewMatrix);
    currentScene->Render(&program);
    if (currentScene != sceneList[0]) {
        switch (lives)
        {
        case 0:
            Util::DrawText(&program, fontTextureID, "Lives: 0", 0.45f, -0.25f,
                glm::vec3(8.0f, -1.0f, 0));
            Util::DrawText(&program, fontTextureID, "Lives: 0", 0.45f, -0.25f,
                glm::vec3(18.0f, -1.0f, 0));
            Util::DrawText(&program, fontTextureID, "Lives: 0", 0.45f, -0.25f,
                glm::vec3(28.0f, -1.0f, 0));
            break;
        case 1:
            Util::DrawText(&program, fontTextureID, "Lives: 1", 0.45f, -0.25f,
                glm::vec3(8.0f, -1.0f, 0));
            Util::DrawText(&program, fontTextureID, "Lives: 1", 0.45f, -0.25f,
                glm::vec3(18.0f, -1.0f, 0));
            Util::DrawText(&program, fontTextureID, "Lives: 1", 0.45f, -0.25f,
                glm::vec3(28.0f, -1.0f, 0));
            break;
        case 2:
            Util::DrawText(&program, fontTextureID, "Lives: 2", 0.45f, -0.25f,
                glm::vec3(8.0f, -1.0f, 0));
            Util::DrawText(&program, fontTextureID, "Lives: 2", 0.45f, -0.25f,
                glm::vec3(18.0f, -1.0f, 0));
            Util::DrawText(&program, fontTextureID, "Lives: 2", 0.45f, -0.25f,
                glm::vec3(28.0f, -1.0f, 0));
            break;
        case 3:
            Util::DrawText(&program, fontTextureID, "Lives: 3", 0.45f, -0.25f,
                glm::vec3(8.0f, -1.0f, 0));
            Util::DrawText(&program, fontTextureID, "Lives: 3", 0.45f, -0.25f,
                glm::vec3(18.0f, -1.0f, 0));
            Util::DrawText(&program, fontTextureID, "Lives: 3", 0.45f, -0.25f,
                glm::vec3(28.0f, -1.0f, 0));
            Util::DrawText(&program, fontTextureID, "Lives: 3", 0.45f, -0.25f,
                glm::vec3(38.0f, -1.0f, 0));
            break;

        }
    }
    for (int i = 1; i < 4; i++) {
       
        if (sceneList[i] == currentScene) {
            switch (i) {
            case 1:
                Util::DrawText(&program, fontTextureID, "Level: 1", 0.45f, -0.25f,
                    glm::vec3(8.0f, -0.5f, 0));
                Util::DrawText(&program, fontTextureID, "Level: 1", 0.45f, -0.25f,
                    glm::vec3(18.0f, -0.5f, 0));
                Util::DrawText(&program, fontTextureID, "Level: 1", 0.45f, -0.25f,
                    glm::vec3(28.0f, -0.5f, 0));
                break;
            case 2:
                Util::DrawText(&program, fontTextureID, "Level: 2", 0.45f, -0.25f,
                    glm::vec3(8.0f, -0.5f, 0));
                Util::DrawText(&program, fontTextureID, "Level: 2", 0.45f, -0.25f,
                    glm::vec3(18.0f, -0.5f, 0));
                Util::DrawText(&program, fontTextureID, "Level: 2", 0.45f, -0.25f,
                    glm::vec3(28.0f, -0.5f, 0));
                break;
            case 3:
                Util::DrawText(&program, fontTextureID, "Level: 3", 0.45f, -0.25f,
                    glm::vec3(8.0f, -0.5f, 0));
                Util::DrawText(&program, fontTextureID, "Level: 3", 0.45f, -0.25f,
                    glm::vec3(18.0f, -0.5f, 0));
                Util::DrawText(&program, fontTextureID, "Level: 3", 0.45f, -0.25f,
                    glm::vec3(28.0f, -0.5f, 0));
                Util::DrawText(&program, fontTextureID, "Level: 3", 0.45f, -0.25f,
                    glm::vec3(38.0f, -0.5f, 0));
                break;
            }
        }
    }
    if (missionCompleted) {
        Util::DrawText(&program, fontTextureID, "Mission Completed", 0.45f, -0.25f,
            currentScene->state.player->position);
    }
    if (missionFailed) {
        Util::DrawText(&program, fontTextureID, "Mission Failed", 0.45f, -0.25f,
            currentScene->state.player->position);
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
        if (currentScene->state.nextScene > 0) {
            SwitchToScene(sceneList[currentScene->state.nextScene]);
            Mix_PlayChannel(-1, kill, 0);
        }
        else if (currentScene->state.nextScene == 0) {
            missionCompleted = true;
            Mix_PlayChannel(-1, kill, 0);
        }
        Render();
    }

    Shutdown();
    return 0;
}