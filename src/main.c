#include "raylib.h"
#include "resource_dir.h"
#include "scene_manager.h"
#include "title_screen.h"

typedef struct DummyStruct
{
	int dummyField;
	char* message;
} DummyStruct;;

void dummyUpdate(void* ctx) {
	DummyStruct* context = (DummyStruct*)ctx;
	if (context->dummyField < 100) context->dummyField += 10;
	printf("Dummy update function called. Dummy field updated to %d \n", context->dummyField);
}

void dummyRender(void* ctx) {
	DummyStruct* context = (DummyStruct*)ctx;
	printf("Dummy render function called.\n");
	DrawText(context->message,
		GetScreenWidth() / 2 - MeasureText(context->message, 20) / 2,
		GetScreenHeight() / 2, 20,
		LIGHTGRAY);
}

int main ()
{
	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);

	InitWindow(1280, 800, "Hello Raylib");

	SearchAndSetResourceDir("resources");

	Texture wabbit = LoadTexture("wabbit_alpha.png");

	SceneStack* globalSceneStack = InitSceneStack();
	//if(globalSceneStack)
	//{
	//	printf("Managed to initialize global scene stack\n");
	//	DummyStruct context;
	//	printf("Allocated memory for TitleScreenContext\n");

	//	context.dummyField = 42;
	//	context.message = "This is a dummy scene!";

	//	printf("Loaded logo texture and set message\n");

	//	Scene titleScene;

	//	titleScene.Update = dummyUpdate; //UpdateTitleScreen;
	//	titleScene.Render = dummyRender; //RenderTitleScreen;
	//	titleScene.ctx = &context;

	//	PushScene(globalSceneStack, &titleScene);
	//	printf("Pushed empty scene onto stack\n");

	//	//return 0;
	//}

	PushScene(globalSceneStack, CreateTitleScreenScene());
	
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

			ClearBackground(BLACK);
			DrawText("Hello Raylib", 200,200,20,WHITE);
			DrawTexture(wabbit, 400, 200, WHITE);
			if (currentScene)
			{
				currentScene->Render(currentScene->ctx);
			}
		
		EndDrawing();
	}

	UnloadTexture(wabbit);
	CloseWindow();

	return 0;
}
