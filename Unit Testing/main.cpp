#include <Windows.h>
#include "Entity.h"

//using float4x4_identity = float4x4{ 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };

int main(int argc, char* argv[]) 
{
	Entity* entity = new Entity();
	
	entity->add_component<NameComponent>("Dan");
	entity->add_component<HealthComponent>(100, 100);
	entity->add_component<TransformComponent>(float4x4{1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1});

	entity->update();

	entity->render();

	delete entity;
	return 0;
}