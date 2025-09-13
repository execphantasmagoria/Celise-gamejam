#include "title_screen.h"
#include "resource_dir.h"
Scene* CreateTitleScreenScene()
{
	TitleScreenContext context;
	if (context)
	{
		SearchAndSetResourceDir("resources");
		context->frameCount = 0;
		context->logo = LoadTexture("portrait.jpg");
		context->message = "Press ENTER or any key to start";
		Scene* scene = (Scene*)malloc(sizeof(Scene));
		if (scene)
		{
			scene->Update = UpdateTitleScreen;
			scene->Render = RenderTitleScreen;
			scene->Free = FreeTitleScreen;
			scene->ctx = context;
			return scene;
		}
		else
		{
			printf("Failed to allocate memory for Scene\n");
			UnloadTexture(context->logo);
			free(context);
			return NULL;
		}
	}
	else
	{
		printf("Failed to allocate memory for TitleScreenContext\n");
		return NULL;
	}
}

void UpdateTitleScreen(void* ctx)
{
	printf("Updating Title Screen...\n");
	TitleScreenContext* context = (TitleScreenContext*)ctx;
	context->frameCount++;
	if (IsKeyPressed(KEY_ENTER) || GetKeyPressed() != 0)
	{
		// Transition to the next scene, e.g., main game scene
		//PopScene(globalSceneStack);
		//PushScene(globalSceneStack, CreateMainGameScene());
		// For now, just print a message
		printf("Transitioning to the main game scene...\n");
	}
	printf("Title Screen updated. Frame count: %d\n", context->frameCount);
}

void RenderTitleScreen(void* ctx)
{
	TitleScreenContext* context = (TitleScreenContext*)ctx;
	DrawTexture(context->logo,
				GetScreenWidth()/2 - context->logo.width/2,
				GetScreenHeight()/2 - context->logo.height/2 - 50,
				WHITE);

	DrawText(context->message,
		GetScreenWidth() / 2 - MeasureText(context->message, 20) / 2,
		GetScreenHeight() / 2 + context->logo.height / 2 + 20, 20,
		LIGHTGRAY);
}

void FreeTitleScreen(void* ctx)
{
	TitleScreenContext* context = (TitleScreenContext*)ctx;
	UnloadTexture(context->logo);
	free(context);
}
