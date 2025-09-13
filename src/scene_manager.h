#ifndef SCENE_MANAGER_H
#define SCENE_MANAGER_H

#define MAX_SCENES 10

typedef struct Scene {
	void (*Update) (void* ctx);
	void (*Render) (void* ctx);
	void* ctx;
} Scene;

typedef struct SceneStack {
	Scene* scenes[MAX_SCENES];
	int scene_count;
	int top;
} SceneStack;

extern SceneStack* globalSceneStack;

SceneStack* InitSceneStack();

void PushScene(SceneStack* stack, Scene* scene);

void PopScene(SceneStack* stack);

Scene* GetCurrentScene(SceneStack* stack);

#endif
