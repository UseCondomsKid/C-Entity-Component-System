# include <stdarg.h>
# include <stddef.h>
# include <stdlib.h>
# include <string.h>
# include <stdio.h>
# include "ecs.h"

Ecs_State ecs_state;

bool ecs_init(u32 component_count, ...)
{
	// Allocation memory for the stack
	ecs_state.stack = malloc(component_count * sizeof(void*));

	if (ecs_state.stack == NULL)
	{
		printf("\nCouldn't allocate memory for the ecs stack");
		return false;
	}

	// Allocation memory for the component store
	ecs_state.component_store.capacity = malloc(component_count * sizeof(u32));
	ecs_state.component_store.top = malloc(component_count * sizeof(u32));
	ecs_state.component_store.sizes = malloc(component_count * sizeof(u32));

	if (ecs_state.component_store.sizes == NULL)
	{
		printf("\nCouldn't allocate memory for the components' sizes");
		return false;
	}

	ecs_state.component_store.count = component_count;

	// Looping through the provided components and allocating memory for each component
	va_list ap;
	va_start(ap, component_count);
	for (int i = 0; i < component_count; ++i)
	{
		size_t component_size = va_arg(ap, size_t);
		
		ecs_state.component_store.sizes[i] = component_size;
		ecs_state.component_store.capacity[i] = ECS_INITIAL_COMPONENT_COUNT;
		ecs_state.component_store.top[i] = 0;

		ecs_state.stack[i] = malloc(ECS_INITIAL_COMPONENT_COUNT * component_size);

		if (ecs_state.stack[i] == NULL)
		{
			printf("\nCouldn't allocate memory for component of id: %d", i);
			return false;
		}
	}
	va_end(ap);

	return true;
}

void ecs_shutdown(void)
{
	// Components
	free(ecs_state.component_store.sizes);
	ecs_state.component_store.sizes = NULL;
	free(ecs_state.component_store.capacity);
	ecs_state.component_store.capacity = NULL;
	free(ecs_state.component_store.top);
	ecs_state.component_store.top = NULL;
	for (int i = 0; i < ecs_state.component_store.count; i++)
	{
		free(ecs_state.stack[i]);
		ecs_state.stack[i] = NULL;
	}
	ecs_state.component_store.count = 0;

	// Stack
	free(ecs_state.stack);
	ecs_state.stack = NULL;
}

bool ecs_scene_push(void)
{
	// Initializing the entity store, and allocating memory
	ecs_state.entity_store.count = 0;
	ecs_state.entity_store.cap = ECS_INITIAL_ENTITY_COUNT;
	ecs_state.entity_store.masks = malloc(ECS_INITIAL_ENTITY_COUNT * sizeof(u32));
	ecs_state.entity_store.flags = malloc(ECS_INITIAL_ENTITY_COUNT * sizeof(u32));

	// Initializing the query result
	ecs_state.query_result.count = 0;
	ecs_state.query_result.entities = malloc(ECS_INITIAL_ENTITY_COUNT * sizeof(u32));

	if (ecs_state.entity_store.masks == NULL ||
			ecs_state.entity_store.flags == NULL ||
			ecs_state.query_result.entities == NULL)
	{
		printf("\nCouldn't allocate memory for entites");
		return false;
	}

	return true;
}
void ecs_scene_pop(void)
{
	// Entities
	// Resetting
	ecs_state.entity_store.count = 0;
	ecs_state.entity_store.cap = 0;
	// Freeing resources
	free(ecs_state.entity_store.flags);
	ecs_state.entity_store.flags = NULL;
	free(ecs_state.entity_store.masks);
	ecs_state.entity_store.masks = NULL;

	// Query result
	free(ecs_state.query_result.entities);
	ecs_state.query_result.entities = NULL;
}

Entity ecs_create_entity(void)
{
	u32 id = ecs_state.entity_store.count;

	// We have reached the end of the entity array, so we resize
	if (id == ecs_state.entity_store.cap)
	{
		printf("\n>> Max entities reached, reallocating");
		size_t new_size = 2 * ecs_state.entity_store.cap * sizeof(u32);
		printf("\n>> New Size: %u", ECS_ENTITY_EXPAND_MULTIPLIER * ecs_state.entity_store.cap);
		// Reallocating entity data
		ecs_state.entity_store.masks = realloc(ecs_state.entity_store.masks, new_size);
		ecs_state.entity_store.flags = realloc(ecs_state.entity_store.flags, new_size);
		// Reallocating ecs_query_result data
		ecs_state.query_result.entities = realloc(ecs_state.query_result.entities, new_size);

		if (ecs_state.entity_store.masks == NULL ||
				ecs_state.entity_store.flags == NULL ||
				ecs_state.query_result.entities == NULL)
		{
			// Couldn't reallocate memory
			printf("\nCouldn't reallocate memory");
			return -1;
		}
		ecs_state.entity_store.cap *= ECS_ENTITY_EXPAND_MULTIPLIER;
	}

	ecs_state.entity_store.count++;
	ecs_state.entity_store.masks[id] = 0;
	ecs_state.entity_store.flags[id] = ENTITY_FLAG_ALIVE;

	return id;
}

void* ecs_get_component(Entity entity_id, u32 component_id)
{
	return (u8*)ecs_state.stack[component_id] + (ecs_state.component_store.sizes[component_id] * entity_id);
}

bool ecs_has_component(Entity entity_id, u32 component_id)
{
	return (ecs_state.entity_store.masks[entity_id] & (1 << component_id)) != 0;
}

bool ecs_add_component(Entity entity_id, u32 component_id, void* data)
{
	if (ecs_state.stack[component_id] == NULL)
	{
		// Component wasn't initialized, check the provided id
		printf("\n>> Component wasn't initialized, check the provided id");
		return false;
	}

	if (ecs_has_component(entity_id, component_id))
	{
		// Already has the component
		printf("\n>> Entity:%u already has the component:%u", entity_id, component_id);
		return false;
	}

	// Checking if we are out of memory, if so we reallocate
	if (ecs_state.component_store.capacity[component_id] == ecs_state.component_store.top[component_id])
	{
		size_t new_size = ecs_state.component_store.capacity[component_id] *
			ECS_COMPONENT_EXPAND_MULTIPLIER *
			ecs_state.component_store.sizes[component_id];

		ecs_state.stack[component_id] = realloc(ecs_state.stack[component_id], new_size);

		if (ecs_state.stack[component_id] == NULL)
		{
			// Couldn't reallocate memory
			printf("\n>> Couldn't reallocate memory for component:%u", component_id);
			return false;
		}
		ecs_state.component_store.capacity[component_id] *= ECS_COMPONENT_EXPAND_MULTIPLIER;
	}

	size_t component_size = ecs_state.component_store.sizes[component_id];
	void* dest = ecs_get_component(entity_id, component_id);

	if (memcpy(dest, data, component_size) == NULL)
	{
		// Couldn't copy component data to memory
		printf("\n>> Couldn't copy component data to memory");
		return false;
	}

	ecs_state.entity_store.masks[entity_id] |= (1 << component_id);
	ecs_state.component_store.top[component_id]++;

	return true;
}

void ecs_remove_component(Entity entity_id, u32 component_id)
{
	ecs_state.entity_store.masks[entity_id] &= ~(1 << component_id);
	// For now, I'm not freeing the entity's data from the components
}

void ecs_kill_entity(Entity entity_id)
{
	// Check if it is alive
	if ((ecs_state.entity_store.flags[entity_id] & ENTITY_FLAG_ALIVE) != 0)
	{
		// Set it to dead
		ecs_state.entity_store.flags[entity_id] &= ~ENTITY_FLAG_ALIVE;
		// Reset the mask
		ecs_state.entity_store.masks[entity_id] = 0;
	}
	else
	{
		// Entity already dead
		return;
	}
}

Ecs_Query_Result* ecs_query(u32 component_count, ...)
{
	va_list ap;
	u32 i, mask = 0;

	ecs_state.query_result.count = 0;

	va_start(ap, component_count);
	for (i = 0; i < component_count; ++i)
	{
		mask |= (1 << va_arg(ap, u32));
	}
	va_end(ap);

	for(i = 0; i < ecs_state.entity_store.count; ++i)
	{
		if ((ecs_state.entity_store.flags[i] & ENTITY_FLAG_ALIVE) != 0 &&
				mask == (ecs_state.entity_store.masks[i] & mask))
		{
			ecs_state.query_result.entities[ecs_state.query_result.count++] = i;
		}
	}

	return &ecs_state.query_result;
}

