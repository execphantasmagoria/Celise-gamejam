#include "raylib.h"
#include "resource_dir.h"
#include "title_screen.h"
#include "Logger.h"

TitleScreenContext title_screen_context = { 0 };
Scene title_scene = { 0 };

//MainMenuContext main_menu_context = { 0 };
//Scene main_menu_scene = { 0 };

int main ()
{
	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);
	InitWindow(1920, 1080, "Hello Raylib");
	
	SearchAndSetResourceDir("resources");

	SceneStack* globalSceneStack = InitSceneStack();

	PushScene(globalSceneStack, CreateTitleScreenScene(title_screen_context, title_scene));
	
	while (!WindowShouldClose())
	{
		Scene* currentScene = GetCurrentScene(globalSceneStack);
		if(currentScene)
		{
			printf("Current scene exists after push!\n");
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
