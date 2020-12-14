#pragma once
#include "Menu.h"
#define MENU_WIDTH 14
#define MENU_HEIGHT 8

unsigned int menu_data[] =
{
1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
};
GLuint fontTextID;
void Menu::Initialize() {
    //GLuint mapTextureID = Util::LoadTexture("tileset.png");
    state.nextScene = -1;
    GLuint mapTextureID = Util::LoadTexture("platformPack_tile001.png");
    fontTextID = Util::LoadTexture("font1.png");
    state.map = new Map(MENU_WIDTH, MENU_HEIGHT, menu_data, mapTextureID, 1.0f, 4, 1);
    state.player = new Entity();
    state.player->entityType = PLAYER;


}
void Menu::Update(float deltaTime) {
}
void Menu::Render(ShaderProgram* program) {
    //state.map->Render(program);
    Util::DrawText(program, fontTextID, "Welcome to Pathfinder! Press Enter to Start", 0.45f, -0.25f,
        glm::vec3(1.0f, -1.0f, 0));
    Util::DrawText(program, fontTextID, "Avoid all Enemies.", 0.45f, -0.25f,
        glm::vec3(1.0f, -2.0f, 0));
    Util::DrawText(program, fontTextID, "Collect all Coins.", 0.45f, -0.25f,
        glm::vec3(1.0f, -3.0f, 0));
    Util::DrawText(program, fontTextID, "Find the Door.", 0.45f, -0.25f,
        glm::vec3(1.0f, -4.0f, 0));
}
