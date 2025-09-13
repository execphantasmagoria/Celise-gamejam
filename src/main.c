#include "raylib.h"
#include "resource_dir.h"
#include "scene_manager.h"

int main ()
{
	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);

	// test
	// 222
	// can you see this kindred?

	InitWindow(1280, 800, "Hello Raylib");

	SearchAndSetResourceDir("resources");

	Texture wabbit = LoadTexture("wabbit_alpha.png");
	
	while (!WindowShouldClose())
	{
		BeginDrawing();

			ClearBackground(BLACK);
			DrawText("Hello Raylib", 200,200,20,WHITE);
			DrawTexture(wabbit, 400, 200, WHITE);
		
		EndDrawing();
	}

	UnloadTexture(wabbit);
	CloseWindow();

	return 0;
}
