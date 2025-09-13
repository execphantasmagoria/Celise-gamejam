#include "title_screen.h"
#include "main_menu.h"
Scene* CreateTitleScreenScene(TitleScreenContext context, Scene scene)
{
	context.frameCount = 0;
	context.logo = LoadTexture("test.png");
	context.message = "Press ENTER or any key to start";

	scene.Update = UpdateTitleScreen;
	scene.Render = RenderTitleScreen;
	scene.ctx = &context;

	return &scene;

}

void UpdateTitleScreen(void* ctx)
{
	TitleScreenContext* context = (TitleScreenContext*)ctx;

	context->frameCount++;
	if (IsKeyPressed(KEY_ENTER) || GetKeyPressed() != 0)
	{
		printf("Transitioning to the main game scene...\n");
		//PopScene(stack); // Remove the title screen
		//PushScene(stack, CreateMainMenu(mainctx, scene)); // Push the main menu scene
	}
	printf("Title Screen updated. Frame count: %d\n", context->frameCount);
}

void RenderTitleScreen(void* ctx)
{
	TitleScreenContext* context = (TitleScreenContext*)ctx;

	ClearBackground(BLACK);

	DrawTexture(context->logo,
		GetScreenWidth() / 2 - context->logo.width / 2,
		GetScreenHeight() / 2 - context->logo.height / 2 - 50,
		WHITE);

	DrawText(context->message,
		GetScreenWidth() / 2 - MeasureText(context->message, 20) / 2,
		GetScreenHeight() / 2 + context->logo.height / 2 + 20, 20,
		LIGHTGRAY);
	printf("Title Screen rendered.\n");
}
