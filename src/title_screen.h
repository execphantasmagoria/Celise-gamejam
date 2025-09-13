#include "scene_manager.h"
#include "main_menu.h"
#include "raylib.h"

typedef struct TitleScreenContext {
	int frameCount;
	Texture2D logo;
	const char* message;
} TitleScreenContext;

Scene* CreateTitleScreenScene(TitleScreenContext ctx, Scene scene);
void UpdateTitleScreen(void* ctx);
void RenderTitleScreen(void* ctx);