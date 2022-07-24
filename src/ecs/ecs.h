# ifndef ECS
# define ECS

# include <stdint.h>
# include <stdbool.h>

typedef uint32_t u32;
typedef uint8_t u8;

typedef u32 Entity;

// Used to store the entities and their properties
typedef struct entity_store
{
	u32* masks; // A list of entity masks, these determine what components the entity has
	u32* flags; // A list of entity flags, there are two flags: Alive:1 and dead:0
	u32 count;  // The number of created entities
	u32 cap;    // The number of allocated entities

}Entity_Store;

// Used to store the components and their properties
typedef struct component_store
{
	u32 count;      // The number of components
	u32* sizes;     // A list that stores the size of each component
	u32* capacity;  // A list that stores the number of component slots are currently used for each component
	u32* top;       // A list that stores the number of allocated component spaces for each component

}Component_Store;

// This struct gets returned whenever you query
typedef struct ecs_query_result
{
	u32 count;        // How many entities have the components
	Entity* entities; // A list of entity ids that have the components

}Ecs_Query_Result;


typedef struct ecs_state
{
	void** stack;                    // A list of pointers to the components
	Entity_Store entity_store;       // Entity store
	Component_Store component_store; // Components store
	Ecs_Query_Result query_result;   // Query result

}Ecs_State;


# define ENTITY_FLAG_ALIVE 1

// The number of spaces to allocate for each component when initializing
# define ECS_INITIAL_COMPONENT_COUNT 10
# define ECS_COMPONENT_EXPAND_MULTIPLIER 2

// The number of spaces to allocate for enities when initializing
# define ECS_INITIAL_ENTITY_COUNT 10
# define ECS_ENTITY_EXPAND_MULTIPLIER 2


/**
 * Initializes the ecs module
 *
 * @param component_count is the number of components that will be provided
 * @param ... the size of each component
 * @return returns true on success, false on failure
 */
bool ecs_init(u32 component_count, ...);
/**
 * Shuts down the ecs module
 */
void ecs_shutdown(void);

/**
 * Resets and allocates the entity data when entering a new scene
 *
 * @return returns true on success, false on failure
 */
bool ecs_scene_push(void);
/**
 * frees the entity data when exiting a scene
 *
 * @return returns true on success, false on failure
 */
void ecs_scene_pop(void);

/**
 * Creates an entity
 *
 * @return returns the id of the created entity
 */
Entity ecs_create_entity(void);
/**
 * Gets the component data of an entity
 *
 * @param entity_id the id of the entity
 * @param component_id the id of the component
 * @return returns a pointer to the data
 */
void* ecs_get_component(Entity entity_id, u32 component_id);
/**
 * Checks if an entity has a component
 *
 * @param entity_id the id of the entity
 * @param component_id the id of the component
 * @return returns true if the entity has the component, false if not
 */
bool ecs_has_component(Entity entity_id, u32 component_id);
/**
 * Adds a component to an entiy
 *
 * @param entity_id the id of the entity
 * @param component_id the id of the component
 * @param data a pointer to the component's data
 * @return returns true if the component is added, false if not
 */
bool ecs_add_component(Entity entity_id, u32 component_id, void* data);
/**
 * Removes a component from an entity
 *
 * @param entity_id the id of the entity
 * @param component_id the id of the component
 */
void ecs_remove_component(Entity entity_id, u32 component_id);
/**
 * Kill an entity, removing all of its components
 *
 * @param entity_id the id of the entity
 */
void ecs_kill_entity(Entity entity_id);
/**
 * Queries for entities that have the provided components
 *
 * @param component_count the number of component that you'll provide
 * @param ... the ids of the components
 * @return returns a pointer to a populated Ecs_Query_Result struct
 */
Ecs_Query_Result* ecs_query(u32 component_count, ...);

# endif
