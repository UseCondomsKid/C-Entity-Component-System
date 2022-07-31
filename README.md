# C99 Entity Component System

#### A simple ecs writen in c99.
---
This ecs is largely inspired from this ecs by Dylan Flaconer
Link: https://gitlab.com/Falconerd/ember-ecs

#### How to compile
This ecs is simply two files [ecs.h](src/ecs/ecs.h) and [ecs.c](src/ecs/ecs.c).
Just add them to your project and include them, nothing crazy here.
#### How to use
Just include [ecs.h](src/ecs/ecs.h) in your project.
All the code is commented, so it should give you an idea of how it all works.
Below is an example of how to use the module.
##### Creating components
Components can be any data type, here I'll create 2 test components.
- Position Component
```c
typedef struct position_component
{
	int x;
	int y;
}Position_Component;
```
- Character Component
```c
typedef struct character_component
{
	char* name;
	u8 age;
	u8 height;
	u8 weight;
}Character_Component;
```
`u8` is a `uint8_t` unsigned 8 bit integer.
##### Initializing the ECS
We use the 2 functions belwo to initialize the ecs, check [ecs.h](src/ecs/ecs.h) for their documentaion.
Initializing the ecs will allocate memory for the components and the entities.
```c
ecs_init(2, sizeof(Position_Component), sizeof(Character_Component));
ecs_scene_push();
```
One note here is that the order in which each component is initialized matters, so in the example above, the position component will have id 0 since it was passed first and the character component will have id 1.
The ids are used to reference the components when querying or when getting a component from an entity.

##### Creating systems for the components
Systems query for entities that have the specified components, we'll create 2 example systems.
To query we use the function `ecs_query()`, check [ecs.h](src/ecs/ecs.h) for further documentation.
- Position system
```c
void position_system(void)
{
	u32 i;
	// Querying for entities that have one component which is the position component
	Ecs_Query_Result* qr = ecs_query(1, 0); // 0 is the id of the position component

	// Looping through the entities 
	for (i = 0; i < qr->count; i++)
	{
		Position_Component* pos = (Position_Component*)ecs_get_component(qr->entities[i], 0);

		printf("\nx:%d | y:%d", pos->x, pos->y);
	}
}
```
- Character system
```c
void character_system(void)
{
	u32 i;
	// Querying for entities that have two components position and character components
	Ecs_Query_Result* qr = ecs_query(2, 0, 1);

	// Looping through the entities 
	for (i = 0; i < qr->count; i++)
	{
		Position_Component* pos = (Position_Component*)ecs_get_component(qr->entities[i], 0);
		Character_Component* character = (Character_Component*)ecs_get_component(qr->entities[i], 1);

		printf("\nx:%d     | y:%d", pos->x, pos->y);
		printf("\nname:%s | age:%u | height:%u | weight:%u", character->name,
				character->age, character->height, character->weight);
	}
}
```
`u32` is a `uint32_t` unsigned 32 bit integer.
These systems above are simply an example, obviously is a real game, you'd want more than to print a system's properties.
##### Creating entities
Creating entities is straight forward, just call the function `ecs_create_entity()` and it will return the id of the entity.
```c
u32 entity1 = ecs_create_entity();
u32 entity2 = ecs_create_entity();
```
##### Adding components to entities
Adding components to entities is also straight forward, you just use this function `ecs_add_component()` (again, check out [ecs.h](src/ecs/ecs.h) for more).
```c
// Adding a position component to entity1
Position_Component position = { .x = 100, .y = -50 };
ecs_add_component(entity1, POSITION_COMPONENT, &position);

// Adding a position component and a character component to entity2
Character_Component character = { .name = "Hero", .age = 23, .height = 189, .weight = 75 };
ecs_add_component(entity2, POSITION_COMPONENT, &position); // reusing the position component already created
ecs_add_component(entity2, CHARACTER_COMPONENT, &character);
```
##### Calling the systems
You'd do this in a proper game loop, but it just amounts to calling the previously declared and implemented functions `void position_system()` and `void character_system()`
```c
while (game_loop)
{
	position_system();
	character_system();
}
```
##### Shutting down the ECS
The ecs needs to be shutdown to free all allocated memory by the module.
```c
ecs_scene_pop();
ecs_shutdown();
```
