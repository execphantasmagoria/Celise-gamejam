#define MAX_SCENES 10
#define NULL 0

typedef struct Scene {
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

SceneStack* InitSceneStack();

void PushScene(SceneStack* stack, Scene* scene);

void PopScene(SceneStack* stack);

Scene* GetCurrentScene(SceneStack* stack);
