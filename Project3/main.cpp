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
#define PLATFORM_COUNT 34

struct GameState {
    Entity* player;
    Entity* platforms;
    Entity* success;
    Entity* fail;
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
    state.player->acceleration = glm::vec3(0, -0.1f, 0);
    state.player->position = glm::vec3(0,4.0f,0);
    state.player->movement = glm::vec3(0);
    state.player->speed = 1.0f;
    state.player->textureID = LoadTexture("playerShip2_green.png");
    fontTextureID = LoadTexture("font1.png");
    state.player->width = 0.75f;
    state.player->height = 0.9;

    state.platforms = new Entity[PLATFORM_COUNT];
    GLuint platformTextureID = LoadTexture("platformPack_tile001.png");
    GLuint correctplatformTextureID = LoadTexture("platformPack_tile019.png");

    float x = -4.5f;
    //floor tiles
    for (int i = 0; i < 10; i++) {
        if (i == 4) {
            state.platforms[i].textureID = correctplatformTextureID;
        }
        else if (i == 3) {
            state.platforms[i].textureID = correctplatformTextureID;
        }
        else {
            state.platforms[i].textureID = platformTextureID;
        }
        state.platforms[i].position = glm::vec3(x, -3.25f, 0);
        x = x + 1.0f;
    }
    
    //left wall tiles
    float y=-4.25f;
    for (int i = 10; i < 20; i++) {
        state.platforms[i].textureID = platformTextureID;
        state.platforms[i].position = glm::vec3(-4.5f, y, 0);
        y = y + 1.0f;
    }
    y = -4.25f;
    //right wall tiles
    for (int i = 20; i < 30; i++) {
        state.platforms[i].textureID = platformTextureID;
        state.platforms[i].position = glm::vec3(4.5f, y, 0);
        y = y + 1.0f;
    }
    
    //hanging tiles
    x = 0.5f;
    for (int i = 30; i < 32; i++) {
        state.platforms[i].textureID = platformTextureID;
        state.platforms[i].position = glm::vec3(x, 0.0f, 0);
        x = x + 1.0f;
    }
    //random tiles
    state.platforms[32].textureID = platformTextureID;
    state.platforms[32].position = glm::vec3(-3.5, 0.25f, 0);
    state.platforms[33].textureID = platformTextureID;
    state.platforms[33].position = glm::vec3(-2.5, 0.25f, 0);

    for (int i = 0; i < PLATFORM_COUNT; i++) {
        state.platforms[i].Update(0, NULL, 0);
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
                // Move the player left
                break;

            case SDLK_RIGHT:
                // Move the player right
                break;

            case SDLK_SPACE:

                break;
            }
            break; // SDL_KEYDOWN
        }
    }

    const Uint8* keys = SDL_GetKeyboardState(NULL);

    if (keys[SDL_SCANCODE_LEFT]) {
        if (!missionCompleted && !missionFailed) {
            state.player->acceleration.x += -0.05f;
        }
    }
    else if (keys[SDL_SCANCODE_RIGHT]) {
        if (!missionCompleted && !missionFailed) {
            state.player->acceleration.x += 0.05f;
        }
    }


    if (glm::length(state.player->movement) > 1.0f) {
        state.player->movement = glm::normalize(state.player->movement);
    }

}

#define FIXED_TIMESTEP 0.0166666f
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
    //std::cout << "running?" << std::endl;
    if (state.player->collidedWith != NULL) {
        if ((state.player->collidedWith == &state.platforms[4]) || (state.player->collidedWith == &state.platforms[3])) {
            missionCompleted = true;
            //std::cout << "mission complete" << std::endl;
            //state.player->acceleration = glm::vec3(0, 0.0f, 0);
            //state.player->speed = 0;
        }
        else {
            missionFailed = true;
            //std::cout << "mission failed" << std::endl;
            //state.player->acceleration = glm::vec3(0, 0.0f, 0);
            //state.player->speed = 0;
        }
        return;
    }
    while (deltaTime >= FIXED_TIMESTEP) {
        // Update. Notice it's FIXED_TIMESTEP. Not deltaTime
        state.player->Update(FIXED_TIMESTEP, state.platforms, PLATFORM_COUNT);

        deltaTime -= FIXED_TIMESTEP;
    }
    accumulator = deltaTime;

    /*
    for (int i = 0; i < PLATFORM_COUNT; i++) {
        if (i != 4 && i != 5) {
            if (state.player->CheckCollision(&state.platforms[i])) {
                std::cout << "collision detected" << std::endl;
                missionFailed = true;
                state.player->acceleration = glm::vec3(0, 0.0f, 0);
            }
        }
    }
    */

}


void Render() {
    glClear(GL_COLOR_BUFFER_BIT);

    for (int i = 0; i < PLATFORM_COUNT; i++) {
        state.platforms[i].Render(&program);
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