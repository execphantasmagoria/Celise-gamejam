#include "main_menu.h"

Scene* CreateMainMenu(MainMenuContext ctx, Scene scene)
{
	ctx.frameCount = 0;
	ctx.background = LoadTexture("background.png");

	//ctx.selectedOption = -1;
	//ctx.logo = LoadTexture("main_menu/logo.png");

	//ctx.buttonStart = LoadTexture("main_menu/button_start.png");
	//ctx.buttonOptions = LoadTexture("main_menu/button_options.png");
	//ctx.buttonExit = LoadTexture("main_menu/button_exit.png");

	//ctx.buttonStartHover = LoadTexture("main_menu/button_start_hover.png");
	//ctx.buttonOptionsHover = LoadTexture("main_menu/button_options_hover.png");
	//ctx.buttonExitHover = LoadTexture("main_menu/button_exit_hover.png");

	//ctx.buttonStartPressed = LoadTexture("main_menu/button_start_pressed.png");
	//ctx.buttonOptionsPressed = LoadTexture("main_menu/button_options_pressed.png");
	//ctx.buttonExitPressed = LoadTexture("main_menu/button_exit_pressed.png");
	//ctx.cursor = LoadTexture("main_menu/cursor.png");

	scene.Update = UpdateMainMenu;
	scene.Render = RenderMainMenuScreen;
	scene.ctx = &ctx;
	return &scene;
}

void UpdateMainMenu(void* ctx)
{
	MainMenuContext* context = (MainMenuContext*)ctx;
	context->frameCount++;
	if (IsKeyPressed(KEY_ENTER))
	{
		printf("Transitioning to the main game scene...\n");
	}
	printf("Main Menu Screen updated. Frame count: %d\n", context->frameCount);
}

void RenderMainMenuScreen(void* ctx)
{
	MainMenuContext* context = (MainMenuContext*)ctx;

	ClearBackground(BLACK);

	DrawTexture(context->background,
		GetScreenWidth() / 2 - context->background.width / 2,
		GetScreenHeight() / 2 - context->background.height / 2 - 50,
		WHITE);

	printf("Main Menu rendered.\n");
}
