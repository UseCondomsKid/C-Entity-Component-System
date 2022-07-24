# include "ecs/ecs.h"
# include <stdio.h>

// Example Components
# define POSITION_COMPONENT 0
typedef struct position_component
{
	int x;
	int y;

}Position_Component;

# define CHARACTER_COMPONENT 1
typedef struct character_component
{
	char* name;
	u8 age;
	u8 height;
	u8 weight;

}Character_Component;

// Systems
void position_system(void)
{
	u32 i;
	// Querying for entities that have one component which is the position component
	Ecs_Query_Result* qr = ecs_query(1, POSITION_COMPONENT);

	// Looping through the entities 
	for (i = 0; i < qr->count; i++)
	{
		Position_Component* pos = (Position_Component*)ecs_get_component(qr->entities[i], POSITION_COMPONENT);

		printf("\nx:%d | y:%d", pos->x, pos->y);
	}
}
void character_system(void)
{
	u32 i;
	// Querying for entities that have two components position and character components
	Ecs_Query_Result* qr = ecs_query(2, POSITION_COMPONENT, CHARACTER_COMPONENT);

	// Looping through the entities 
	for (i = 0; i < qr->count; i++)
	{
		Position_Component* pos = (Position_Component*)ecs_get_component(qr->entities[i], POSITION_COMPONENT);
		Character_Component* character = (Character_Component*)ecs_get_component(qr->entities[i], CHARACTER_COMPONENT);

		printf("\nx:%d     | y:%d", pos->x, pos->y);
		printf("\nname:%s | age:%u | height:%u | weight:%u", character->name,
				character->age, character->height, character->weight);
	}
}


int main()
{
	// Initializing ecs
	// Here the position component will have id 0 since it was passed first
	// The character_component will have id 1
	ecs_init(2, sizeof(Position_Component), sizeof(Character_Component));
	ecs_scene_push();

	// Creating two entities
	u32 entity1 = ecs_create_entity();
	u32 entity2 = ecs_create_entity();

	// Adding a position component to entity1
	Position_Component position = { .x = 100, .y = -50 };
	ecs_add_component(entity1, POSITION_COMPONENT, &position);

	// Adding a position component and a character component to entity2
	Character_Component character = { .name = "Hero", .age = 23, .height = 189, .weight = 75 };
	ecs_add_component(entity2, POSITION_COMPONENT, &position);
	ecs_add_component(entity2, CHARACTER_COMPONENT, &character);


	// A loop
	bool exit_flagged = false;
	while (!exit_flagged)
	{
		position_system();
		character_system();
	}

	// Shutting down ecs
	ecs_scene_pop();
	ecs_shutdown();
}


