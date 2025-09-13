#include "scene_manager.h"
#include "raylib.h"

typedef struct TitleScreenContext {
	int frameCount;
	Texture2D logo;
	const char* message;
} TitleScreenContext;

Scene* CreateTitleScreenScene();

void UpdateTitleScreen(void* ctx);
void RenderTitleScreen(void* ctx);
void FreeTitleScreen(void* ctx);