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
	Texture2D bg;
	const char* message;
	Font scene_font;
} TitleScreenContext;

typedef struct MainMenuContext {
	int frameCount;
	bool logoSettled;
	bool buttonSelected;
	float logoY;
	float targetLogoY;
	Texture2D logo;
	Texture2D bg;
	Texture2D newGameButton;
	Texture2D newGameButtonHover;

} MainMenuContext;

typedef struct CeliseCastleContext
{
	int frameCount;
	bool sceneRendered;
	Texture2D bg1;
	Texture2D bg2;
	Texture2D bg3;
} CeliseCastleContext;

typedef struct TopBarContext {
	int frameCount;
	Texture2D bg;
	Texture2D portrait_frame;
	Texture2D portrait;
	Texture2D ui_frame;
	char* character_name;
	Texture2D hp_bar;
	Texture2D hp_fill;
	float hp_percentage;
	Texture2D interact_icon;
	Texture2D inventory_icon;
	Texture2D journal_icon;
	Texture2D daylight_icon;
	Texture2D map_icon;
	Texture2D menu_icon;
	Texture2D gold_icon;
	char* datetime;
	int gold;

} TopBarContext;

typedef struct Player {
	Texture2D spriteSheet;
	int frameWidth;
	int frameHeight;
	int frameCount;
	int currentFrame;
	float frameTime;
	float timer;
	int direction; // 1 for left, -1 for right
	Vector2 position;
} Player;

// ----- Global Declarations & Forward Declarations -----

SceneStack* globalSceneStack;

TitleScreenContext title_screen_context = { 0 };
Scene title_scene = { 0 };
BaseSceneContext base_scene_context = { 0 };
Scene base_scene = { 0 };
MainMenuContext main_menu_context = { 0 };
Scene main_menu_scene = { 0 };
CeliseCastleContext celise_castle_context = { 0 };
Scene prologue_scene = { 0 };
TopBarContext top_bar_context = { 0 };
Scene top_bar_scene = { 0 };

SceneStack* InitSceneStack();
void PushScene(SceneStack* stack, Scene* scene);
void PopScene(SceneStack* stack);
Scene* GetCurrentScene(SceneStack* stack);
Scene* CreateTitleScreenScene(TitleScreenContext* context, Scene* scene);
Scene* CreateBaseScene(BaseSceneContext* context, Scene* scene);
Scene* CreateMainMenuScene(MainMenuContext* context, Scene* scene);
Scene* CreateCastleScene(CeliseCastleContext* context, Scene* scene);
Scene* CreateTopBar(TopBarContext* context, Scene* scene);
Player CreatePlayer(const char* spritePath, int frameWidth, int frameHeight, int frameCount, Vector2 startPos);
void UpdatePlayerAnimation(Player* player);
void DrawPlayer(Player* player);

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
	Scene* topbar = CreateTopBar(&top_bar_context, &top_bar_scene);
	Player player = CreatePlayer("character/walking_sprite_sheet.png", 180, 220, 6, (Vector2) { 100, 350 });
	while (!WindowShouldClose())
	{
		Scene* currentScene = GetCurrentScene(globalSceneStack);
		if (currentScene)
		{
			currentScene->Update(currentScene->ctx);
			topbar->Update(topbar->ctx);
			UpdatePlayerAnimation(&player);
		}
		else
		{
			printf("[DEBUG ERROR] GetCurrentScene(globalSceneStack) failed to return valid scene.\n");
		}

		BeginDrawing();

		if (currentScene)
		{
			currentScene->Render(currentScene->ctx);
			topbar->Render(topbar->ctx);
			DrawPlayer(&player);
		}

		EndDrawing();
	}
	topbar->Free(topbar->ctx);
	CloseWindow();
	return 0;
}

// -----------------------Player -----------------------

Player CreatePlayer(const char* spritePath, int frameWidth, int frameHeight, int frameCount, Vector2 startPos)
{
	Player player = { 0 };
	player.spriteSheet = LoadTexture(spritePath);
	player.frameWidth = frameWidth;
	player.frameHeight = frameHeight;
	player.frameCount = frameCount;
	player.currentFrame = 0;
	player.frameTime = 0.1f; // Time per frame
	player.timer = 0.0f;
	player.position = startPos;
	player.direction = -1; // Initially facing right
	return player;
}

void UpdatePlayerAnimation(Player* player)
{
	bool moving = false;

	if (IsKeyDown(KEY_RIGHT))
	{
		// Prevent moving off right edge
		if (player->position.x < GetScreenWidth() - player->frameWidth)
		{
			player->position.x += 4.0f;
		}
		else
		{
			player->position.x = GetScreenWidth() - player->frameWidth;
		}
		moving = true;
		player->direction = -1; // Facing right
	}
	if (IsKeyDown(KEY_LEFT))
	{
		// Prevent moving off left edge
		if (player->position.x > 0)
		{
			player->position.x -= 4.0f;
		}
		else
		{
			player->position.x = 0;
		}
		player->position.x -= 4.0f;
		moving = true;
		player->direction = 1; // Facing left
	}

	if (!moving)
	{
		player->currentFrame = 1; // Reset to first frame when not moving
		return;
	}

	player->timer += GetFrameTime();
	if (player->timer >= player->frameTime)
	{
		player->currentFrame = (player->currentFrame + 1) % player->frameCount;
		player->timer = 0.0f;
	}
}

void DrawPlayer(Player* player)
{
	Scene* current_scene = GetCurrentScene(globalSceneStack);
	if (current_scene->scene_name == "title_screen" || current_scene->scene_name == "main_menu")
	{
		return; // Skip rendering player in title screen and main menu
	}
	Rectangle sourceRec = { player->currentFrame * player->frameWidth, 0, (float)player->frameWidth * player->direction, (float)player->frameHeight };
	Rectangle destRec = { player->position.x, player->position.y, (float)player->frameWidth, (float)player->frameHeight };
	Vector2 origin = { 0, 0 };
	DrawTexturePro(player->spriteSheet, sourceRec, destRec, origin, 0.0f, WHITE);
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
	return NULL;
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
	context->logo = LoadTexture("logo.png");
	context->bg = LoadTexture("background.png");
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


	DrawTexturePro(context->bg,
		(Rectangle){ 0, 0, (float)context->bg.width, (float)context->bg.height },
		(Rectangle){ 0, 0, (float)GetScreenWidth(), (float)GetScreenHeight() },
		(Vector2){ 0, 0 },
		0.0f,
		WHITE);

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
	UnloadTexture(context->bg);
}

// -------------------- Main Menu --------------------------

void UpdateMainMenu(void* ctx);
void RenderMainMenu(void* ctx);
void UnloadMainMenu(void* ctx);

Scene* CreateMainMenuScene(MainMenuContext* context, Scene* scene)
{
	context->frameCount = 0;
	context->logo = LoadTexture("logo.png");
	context->bg = LoadTexture("background.png");
	context->logoY = GetScreenHeight()/2.0f; // Start off-screen
	context->targetLogoY = GetScreenHeight() / 4.0f; // Target position
	context->logoSettled = false;
	context->buttonSelected = false;
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

	DrawTexturePro(context->bg,
		(Rectangle){ 0, 0, (float)context->bg.width, (float)context->bg.height },
		(Rectangle){ 0, 0, (float)GetScreenWidth(), (float)GetScreenHeight() },
		(Vector2){ 0, 0 },
		0.0f,
		WHITE);

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
	if ( (CheckCollisionPointRec(GetMousePosition(), newGameRect)) || (IsKeyPressed(KEY_DOWN)) || context->buttonSelected) {
		context->buttonSelected = true;
		DrawTexture(context->newGameButtonHover, newGameRect.x, newGameRect.y, WHITE);
		if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) || IsKeyPressed(KEY_ENTER)) {
			PopScene(globalSceneStack);
			PushScene(globalSceneStack, CreateCastleScene(&celise_castle_context, &prologue_scene));
		}
	} else {
		DrawTexture(context->newGameButton, newGameRect.x, newGameRect.y, WHITE);
	}

}

void UnloadMainMenu(void* ctx)
{
	MainMenuContext* context = (MainMenuContext*)ctx;
	UnloadTexture(context->logo);
	UnloadTexture(context->bg);
}

// ----- Prologue Scene

void UpdateCastleScene(void* ctx);
void RenderCastleScene(void* ctx);
void UnloadCastleScene(void* ctx);

Scene* CreateCastleScene(CeliseCastleContext* context, Scene* scene)
{
	context->frameCount = 0;
	context->bg1 = LoadTexture("wall.png");
	context->bg2 = LoadTexture("floor.png");
	context->bg3 = LoadTexture("carpet_red.png");
	context->sceneRendered = false;
	
	scene->ctx = context;
	scene->Update = UpdateCastleScene;
	scene->Render = RenderCastleScene;
	scene->Free = UnloadCastleScene;
	scene->scene_name = "celise_castle_prologue";
}

void UpdateCastleScene(void* ctx)
{
	CeliseCastleContext* context = (CeliseCastleContext*) ctx;

	//if(context->sceneRendered)
	//{
	//	// For demonstration, pop the scene after rendering once
	//	//PopScene(globalSceneStack);
	//	//PushScene(globalSceneStack, CreateTopBar(&top_bar_context, &top_bar_scene));
	//}
}

void RenderCastleScene(void* ctx)
{
	CeliseCastleContext* context = (CeliseCastleContext*)ctx;

	//if(context->sceneRendered)
	//{
	//	return; // Skip rendering if already rendered
	//}

	ClearBackground(BLACK);

	Vector2 origin = { 0, 0 };

	DrawTexturePro(context->bg1,
		(Rectangle) {
			0,
			0,
			context->bg1.width * 3,
			context->bg1.height
		},
		(Rectangle) {
			0,
			GetScreenHeight() / 2 - context->bg1.height / 2 - 70,
			context->bg1.width * 3,
			context->bg1.height
		},
		origin,
		0.0f,
		WHITE);

	DrawTexturePro(context->bg2,
		(Rectangle) {
			0,
			0,
			context->bg2.width * 14,
			context->bg2.height * 3
		},
		(Rectangle){
			0,
			GetScreenHeight() / 2 + context->bg1.height / 2 - 70,
			context->bg2.width * 14,
			context->bg2.height * 5
		},
		origin,
		0.0f,
		WHITE);

	DrawTexturePro(context->bg3,
		(Rectangle) {
			0,
			0,
			context->bg3.width * 14,
			context->bg3.height
		},
		(Rectangle) {
			0,
			GetScreenHeight() / 2 + context->bg1.height / 2 + context->bg3.height / 4 - 70,
			context->bg2.width * 14,
			context->bg3.height + 70
		},
		origin,
		0.0f,
		WHITE);

	context->sceneRendered = true;

}

void UnloadCastleScene(void* ctx)
{
	CeliseCastleContext* context = (CeliseCastleContext*)ctx;

	UnloadTexture(context->bg1);
	UnloadTexture(context->bg2);
	UnloadTexture(context->bg3);
}

// ------------------------- Top Bar ----------------------------

void UpdateTopBar(void* ctx);
void RenderTopBar(void* ctx);
void UnloadTopBar(void* ctx);

Scene* CreateTopBar(TopBarContext* context, Scene* scene)
{
	context->bg = LoadTexture("hud.png");
	context->portrait_frame = LoadTexture("portrait_frame.png");
	context->portrait = LoadTexture("portrait.png");
	context->ui_frame = LoadTexture("ui_frame.png");

	scene->Update = UpdateTopBar;
	scene->Render = RenderTopBar;
	scene->Free = UnloadTopBar;
	scene->scene_name = "top_bar";
	scene->ctx = context;
	return scene;
}

void UpdateTopBar(void* ctx)
{
	TopBarContext* context = (TopBarContext*)ctx;
}

void RenderTopBar(void* ctx)
{
	Scene* current_scene = GetCurrentScene(globalSceneStack);
	if(current_scene->scene_name == "title_screen" || current_scene->scene_name == "main_menu")
	{
		return; // Skip rendering top bar in title screen and main menu
	}

	TopBarContext* context = (TopBarContext*)ctx;

	DrawTexturePro(context->bg,
		(Rectangle) {0, 0, (float)context->bg.width * 14, (float)context->bg.height},
		(Rectangle) {0, 0, (float)context->bg.width * 14, (float)context->bg.height},
		(Vector2) {0, 0},
		0.0f,
		WHITE);

	DrawTexturePro(context->portrait,
		(Rectangle) {0, 0, (float)context->portrait.width, (float)context->portrait.height},
		(Rectangle) {0, 0, (float)context->portrait.width, (float)context->portrait.height},
		(Vector2) {0, 0},
		0.0f,
		WHITE);

	DrawTexturePro(context->portrait_frame,
		(Rectangle) {
		0, 0, (float)context->portrait_frame.width, (float)context->portrait_frame.height},
		(Rectangle) {
		0, 0, (float)context->portrait_frame.width, (float)context->portrait_frame.height},
		(Vector2) {0, 0},
		0.0f,
		WHITE);

	DrawTexturePro(context->ui_frame,
		(Rectangle) {0, 0, (float)context->ui_frame.width, (float)context->ui_frame.height},
		(Rectangle) {0, 0, (float)context->ui_frame.width, (float)context->ui_frame.height},
		(Vector2) {0, 0},
		0.0f,
		WHITE);

	DrawTexturePro(context->ui_frame,
		(Rectangle) { 0, 0, (float)context->ui_frame.width, (float)context->ui_frame.height
	},
		(Rectangle) {
		(float)context->ui_frame.width + 10, 0, (float)context->ui_frame.width + 100, (float)context->ui_frame.height
	},
		(Vector2) {
		0, 0
	},
		0.0f,
		WHITE);
}

void UnloadTopBar(void* ctx)
{
	TopBarContext* context = (TopBarContext*)ctx;

	UnloadTexture(context->bg);
	UnloadTexture(context->portrait_frame);
	UnloadTexture(context->portrait);
	UnloadTexture(context->ui_frame);
}