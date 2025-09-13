#include "scene_manager.h"
#include <stdlib.h>
SceneStack* InitSceneStack()
{
	SceneStack* stack = (SceneStack*)malloc(sizeof(SceneStack));
	if (stack)
	{
		stack->scene_count = 0;
		stack->top = -1;
		return stack;
	}
	else if (stack == NULL)
		{
		perr("Failed to allocate memory for SceneStack\n");)
		return NULL;
	}
	return stack;
}

void PushScene(SceneStack* stack, Scene* scene)
{
}

void PopScene(SceneStack* stack)
{
}

Scene* GetCurrentScene(SceneStack* stack)
{
	return NULL;
}
