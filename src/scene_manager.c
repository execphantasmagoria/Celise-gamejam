#include "scene_manager.h"

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