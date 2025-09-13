#ifndef MAIN_MENU_H
#define MAIN_MENU_H

#include "scene_manager.h"
#include "raylib.h"

typedef struct MainMenuContext {
	int frameCount;
	Texture2D background;

	//int selectedOption;
	//Texture2D logo;

	//Texture2D buttonStart;
	//Texture2D buttonOptions;
	//Texture2D buttonExit;

	//Texture2D buttonStartHover;
	//Texture2D buttonOptionsHover;
	//Texture2D buttonExitHover;

	//Texture2D buttonStartPressed;
	//Texture2D buttonOptionsPressed;
	//Texture2D buttonExitPressed;

	//Texture2D cursor;
} MainMenuContext;

Scene* CreateMainMenu(MainMenuContext ctx, Scene scene);
void UpdateMainMenu(void* ctx);
void RenderMainMenuScreen(void* ctx);

#endif // MAIN_MENU_H