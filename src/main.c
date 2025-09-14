#include "raylib.h"
#include "resource_dir.h"
#define MAX_SCENES 10
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

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
	Font scene_font;
} TitleScreenContext;

typedef struct MainMenuContext {
	int frameCount;
	bool logoSettled;
	float logoY;
	float targetLogoY;
	Texture2D logo;
	Texture2D newGameButton;
	Texture2D newGameButtonHover;

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

// --------------------- Logger ----------------------

void CustomLog(int msgType, const char* text, va_list args)
{
	char timeStr[64] = { 0 };
	time_t now = time(NULL);
	struct tm* tm_info = localtime(&now);

	strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", tm_info);
	printf("[%s] ", timeStr);

	switch (msgType)
	{
	case LOG_INFO: printf("[INFO] : "); break;
	case LOG_ERROR: printf("[ERROR]: "); break;
	case LOG_WARNING: printf("[WARN] : "); break;
	case LOG_DEBUG: printf("[DEBUG]: "); break;
	default: break;
	}

	vprintf(text, args);
	printf("\n");
}

// --------------------- Main Loop ----------------------

int main()
{
	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);
	InitWindow(1280, 720, "Celise");
	SetTargetFPS(60);
	SetTraceLogCallback(CustomLog);

	SearchAndSetResourceDir("resources");

	globalSceneStack = InitSceneStack();

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
			printf("[DEBUG ERROR] GetCurrentScene(globalSceneStack) failed to return valid scene.\n");
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
		printf("[DEBUG ERROR] Failed to allocate memory for SceneStack\n");
		return NULL;
	}
}

void PushScene(SceneStack* stack, Scene* scene)
{
	if (stack->scene_count < MAX_SCENES)
	{
		stack->top++;
		stack->scenes[stack->top] = scene;
		stack->scene_count++;
		printf("\n[DEBUG INFO] Pushed scene: <%s> | Current Scene Count: %d, Top index: %d\n", scene->scene_name, stack->scene_count, stack->top);
	}
}

void PopScene(SceneStack* stack)
{
	if (stack->scene_count > 0)
	{
		Scene* scene = stack->scenes[stack->top];
		if(scene->scene_name == "base_scene")
		{
			printf("[DEBUG WARN] Base scene cannot be popped!\n");
			return;
		}
		printf("[DEBUG INFO] Popping scene: <%s>\n", scene->scene_name);
		stack->top--;
		stack->scene_count--;
		if (scene->Free)
		{
			printf("[DEBUG INFO] Freeing scene resources for <%s>\n", scene->scene_name);
			scene->Free(scene->ctx);
		}
	}
	else
	{
		printf("[DEBUG ERROR] Scene Stack is empty! Current Scene Count: %d\n", stack->scene_count);
	}
}

Scene* GetCurrentScene(SceneStack* stack)
{
	if (stack->scene_count > 0)
	{
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
	context->message = "> Press ENTER or any key to start <";
	context->scene_font = LoadFont("DalelandsUncial-BOpn.ttf");

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

	Vector2 textSize = MeasureTextEx(context->scene_font,
			context->message,
			context->scene_font.baseSize,
		2);

	Vector2 textPos = { GetScreenWidth()/2.0f - textSize.x/2.0f, GetScreenHeight() / 2.0f + context->logo.height / 2.0f + 20.0f}; // 20.0f is spacing between logo and text

	DrawTextEx(context->scene_font,
			context->message,
			textPos,
			context->scene_font.baseSize,
			2,
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
	context->logo = LoadTexture("logo.jpg");
	context->logoY = GetScreenHeight()/2.0f; // Start off-screen
	context->targetLogoY = GetScreenHeight() / 4.0f; // Target position
	context->logoSettled = false;
	context->newGameButton = LoadTexture("ui/my/ng.png");
	context->newGameButtonHover = LoadTexture("ui/my/ng_hover.png");

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

	// Smoothly interpolate logo upwards
	if (!context->logoSettled) {
		context->logoY += (context->targetLogoY - context->logoY) * 0.05f;

		if (fabsf(context->logoY - context->targetLogoY) < 1.0f) {
			context->logoY = context->targetLogoY;
			context->logoSettled = true;
		}
	}

	DrawTexture(context->logo,
		GetScreenWidth() / 2 - context->logo.width / 2,
		(int)context->logoY - context->logo.height / 2,
		WHITE);

	int buttonWidth = context->newGameButton.width;
	int buttonHeight = context->newGameButton.height;
	int spacing = 20; // Space between buttons
	int startY = (int)context->logoY + context->logo.height / 2 + 40; // Start below logo
	// New Game Button

	Rectangle newGameRect = { GetScreenWidth() / 2 - buttonWidth/2, startY, buttonWidth, buttonHeight };
	if (CheckCollisionPointRec(GetMousePosition(), newGameRect)) {
		DrawTexture(context->newGameButtonHover, newGameRect.x, newGameRect.y, WHITE);
		if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
			printf("New Game Clicked!\n");
			// Transition to new game scene here
		}
	} else {
		DrawTexture(context->newGameButton, newGameRect.x, newGameRect.y, WHITE);
	}

}

void UnloadMainMenu(void* ctx)
{
	MainMenuContext* context = (MainMenuContext*)ctx;
	UnloadTexture(context->logo);
}