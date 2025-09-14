#include "raylib.h"
#include "resource_dir.h"
#define MAX_SCENES 10
#include <stdio.h>
#include <stdlib.h>

// -------------- Structure Definitions ----------------

typedef struct Scene {
	char* scene_name;
	void (*Update) (void* ctx);
	void (*Render) (void* ctx);
	void (*Free) (void* ctx);
	void* ctx;
} Scene;

typedef struct SceneStack {
	Scene* scenes[MAX_SCENES];
	int scene_count;
	int top;
} SceneStack;

typedef struct BaseSceneContext {
	int dummy;
} BaseSceneContext;

typedef struct TitleScreenContext {
	int frameCount;
	Texture2D logo;
	const char* message;
} TitleScreenContext;

typedef struct MainMenuContext {
	int frameCount;
	Texture2D logo;
	Texture2D background;
	int selectedOption;
} MainMenuContext;
// ----- Global Declarations & Forward Declarations -----

SceneStack* globalSceneStack;

TitleScreenContext title_screen_context = { 0 };
Scene title_scene = { 0 };
BaseSceneContext base_scene_context = { 0 };
Scene base_scene = { 0 };
MainMenuContext main_menu_context = { 0 };
Scene main_menu_scene = { 0 };

SceneStack* InitSceneStack();
void PushScene(SceneStack* stack, Scene* scene);
void PopScene(SceneStack* stack);
Scene* GetCurrentScene(SceneStack* stack);
Scene* CreateTitleScreenScene(TitleScreenContext* context, Scene* scene);
Scene* CreateBaseScene(BaseSceneContext* context, Scene* scene);
Scene* CreateMainMenuScene(MainMenuContext* context, Scene* scene);


// --------------------- Main Loop ----------------------

int main()
{
	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);
	InitWindow(1920, 1080, "Hello Raylib");

	SearchAndSetResourceDir("resources");

	globalSceneStack = InitSceneStack();

	printf("Scene stack initialized at address %p\n", globalSceneStack);

	PushScene(globalSceneStack, CreateBaseScene(&base_scene_context, &base_scene));
	PushScene(globalSceneStack, CreateTitleScreenScene(&title_screen_context, &title_scene));

	while (!WindowShouldClose())
	{
		Scene* currentScene = GetCurrentScene(globalSceneStack);
		if (currentScene)
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

// ---------------- Scene Management -------------------

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
		printf("\n");
		printf("Current Scene Count before push: %d\n", stack->scene_count);
		printf("Top index before push: %d\n", stack->top);
		stack->top++;
		printf("Pushing scene... <%s> to index %d\n", scene->scene_name, stack->top);
		stack->scenes[stack->top] = scene;
		stack->scene_count++;
		printf("Pushed scene: %s\n", scene->scene_name);
		printf("Current Scene Count: %d\n", stack->scene_count);
		printf("Top index: %d\n", stack->top);
	}
}

void PopScene(SceneStack* stack)
{
	if (stack->scene_count > 0)
	{
		Scene* scene = stack->scenes[stack->top];
		if(scene->scene_name == "base_scene")
		{
			printf("Base scene cannot be popped.\n");
			return;
		}
		printf("\nPopping scene... <%s>\n", scene->scene_name);
		stack->top--;
		stack->scene_count--;
		if (scene->Free)
		{
			printf("Freeing scene resources for <%s>\n", scene->scene_name);
			scene->Free(scene->ctx);
		}
	}
	else
	{
		printf("\nScene Stack is empty. Current Scene Count: %d\n", stack->scene_count);
	}
}

Scene* GetCurrentScene(SceneStack* stack)
{
	if (stack->scene_count > 0)
	{
		printf("\n");
		printf("Current Scene Name: %s\n", stack->scenes[stack->top]->scene_name);
		printf("Scene Update Function Address: %p\n", stack->scenes[stack->top]->Update);
		printf("Scene Render Function Address: %p\n", stack->scenes[stack->top]->Render);
		printf("Getting current scene... Current Scene Count: %d, Top index: %d\n", stack->scene_count, stack->top);
		return stack->scenes[stack->top];
	}
}

// -------------------- Base Scene ---------------------

void UpdateBaseScene(void* ctx);
void RenderBaseScene(void* ctx);
void UnloadBaseScene(void* ctx);

Scene* CreateBaseScene(BaseSceneContext* context, Scene* scene)
{
	context->dummy = 1;
	scene->Update = UpdateBaseScene;
	scene->Render = RenderBaseScene;
	scene->Free = UnloadBaseScene;
	scene->scene_name = "base_scene";
	scene->ctx = context;
	return scene;
}

void UpdateBaseScene(void* ctx)
{
	BaseSceneContext* context = (BaseSceneContext*)ctx;
	if (IsKeyPressed(KEY_ENTER) || GetKeyPressed() != 0)
	{
		PopScene(globalSceneStack);
	}
}

void RenderBaseScene(void* ctx)
{
	ClearBackground(RAYWHITE);
	DrawText("Null Scene", 10, 10, 20, DARKGRAY);
}

void UnloadBaseScene(void* ctx)
{
	// No resources to unload in this example
}

// ------------------- Title Screen Scene ------------------

void UpdateTitleScreen(void* ctx);
void RenderTitleScreen(void* ctx);
void UnloadTitleScreen(void* ctx);

Scene* CreateTitleScreenScene(TitleScreenContext* context, Scene* scene)
{
	context->frameCount = 0;
	context->logo = LoadTexture("logo.jpg");
	context->message = "Press ENTER or any key to start";

	scene->Update = UpdateTitleScreen;
	scene->Render = RenderTitleScreen;
	scene->Free = UnloadTitleScreen;
	scene->scene_name = "title_screen";
	scene->ctx = context;

	return scene;

}

void UpdateTitleScreen(void* ctx)
{
	TitleScreenContext* context = (TitleScreenContext*)ctx;

	context->frameCount++;
	if (IsKeyPressed(KEY_ENTER) || GetKeyPressed() != 0)
	{
		printf("Transitioning to the main game scene...\n");
		printf("Stack address %p", globalSceneStack);
		PopScene(globalSceneStack); // Remove the title screen
		PushScene(globalSceneStack, CreateMainMenuScene(&main_menu_context, &main_menu_scene)); // Push the main menu scene
	}
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
}

void UnloadTitleScreen(void* ctx)
{
	TitleScreenContext* context = (TitleScreenContext*)ctx;

	UnloadTexture(context->logo);
}

// -------------------- Main Menu --------------------------

void UpdateMainMenu(void* ctx);
void RenderMainMenu(void* ctx);
void UnloadMainMenu(void* ctx);

Scene* CreateMainMenuScene(MainMenuContext* context, Scene* scene)
{
	context->frameCount = 0;
	context->background = LoadTexture("background.png");
	context->selectedOption = 0;

	scene->Update = UpdateMainMenu;
	scene->Render = RenderMainMenu;
	scene->Free = UnloadMainMenu;
	scene->scene_name = "main_menu";
	scene->ctx = context;

	return scene;

}

void UpdateMainMenu(void* ctx)
{
	MainMenuContext* context = (MainMenuContext*)ctx;

}

void RenderMainMenu(void* ctx)
{
	MainMenuContext* context = (MainMenuContext*)ctx;

	ClearBackground(BLACK);

	DrawTexture(context->background,
		GetScreenWidth() / 2 - context->logo.width / 2,
		GetScreenHeight() / 2 - context->logo.height / 2 - 50,
		WHITE);

}

void UnloadMainMenu(void* ctx)
{
	MainMenuContext* context = (MainMenuContext*)ctx;
	UnloadTexture(context->logo);
	UnloadTexture(context->background);
}