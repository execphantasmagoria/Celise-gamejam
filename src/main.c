#include "raylib.h"
#include "resource_dir.h"
#define MAX_SCENES 10
#include <stdio.h>
#include <stdlib.h>

// -------------------------------------------------------------------------------------------------------------

typedef struct Scene {
	void (*Update) (void* ctx);
	void (*Render) (void* ctx);
	void* ctx;
} Scene;

typedef struct SceneStack {
	Scene* scenes[MAX_SCENES];
	int scene_count;
	int top;
} SceneStack;

typedef struct TitleScreenContext {
	int frameCount;
	Texture2D logo;
	const char* message;
} TitleScreenContext;
// -------------------------------------------------------------------------------------------------------------

SceneStack* globalSceneStack;

SceneStack* InitSceneStack() {
	SceneStack* stack = (SceneStack*)malloc(sizeof(SceneStack));
	if (stack)
	{
		stack->scene_count = 0;
		stack->top = -1;
		return stack;
	}
	else if (stack == NULL)
	{
		printf("Failed to allocate memory for SceneStack\n");
		return NULL;
	}
}

void PushScene(SceneStack* stack, Scene* scene)
{
	if (stack->scene_count < MAX_SCENES)
	{
		stack->scenes[++stack->top] = scene;
		stack->scene_count++;
	}
}

void PopScene(SceneStack* stack)
{
	if (stack->scene_count > 0)
	{
		Scene* scene = stack->scenes[stack->top--];
		stack->scene_count--;
	}
}

Scene* GetCurrentScene(SceneStack* stack)
{
	if (stack->scene_count > 0)
	{
		return stack->scenes[stack->top];
	}
}

// -------------------------------------------------------------------------------------------------------------

void UpdateTitleScreen(void* ctx);
void RenderTitleScreen(void* ctx);

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
		printf("Stack address %p", globalSceneStack);
		//PopScene(globalSceneStack); // Remove the title screen
		//PushScene(stack, CreateMainMenu(mainctx, scene)); // Push the main menu scene
	}
	//printf("Title Screen updated. Frame count: %d\n", context->frameCount);
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
	//printf("Title Screen rendered.\n");
}


// -------------------------------------------------------------------------------------------------------------

TitleScreenContext title_screen_context = { 0 };
Scene title_scene = { 0 };

int main ()
{
	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);
	InitWindow(1920, 1080, "Hello Raylib");
	
	SearchAndSetResourceDir("resources");

	globalSceneStack = InitSceneStack();

	printf("Scene stack initialized at address %p\n", globalSceneStack);

	PushScene(globalSceneStack, CreateTitleScreenScene(title_screen_context, title_scene));
	
	while (!WindowShouldClose())
	{
		Scene* currentScene = GetCurrentScene(globalSceneStack);
		if(currentScene)
		{
			currentScene->Update(currentScene->ctx);
		}
		else
		{
			printf("No current scene after push!\n");
		}

		BeginDrawing();

			if (currentScene)
			{
				currentScene->Render(currentScene->ctx);
			}
		
		EndDrawing();
	}
	CloseWindow();

	return 0;
}
