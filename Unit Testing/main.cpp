#include <Windows.h>
#include "Entity.h"


int main(int argc, char* argv[]) 
{
	Entity* e1 = new Entity();
	
	e1->add_component<NameComponent>("Dan");
	e1->add_component<HealthComponent>(100, 100);
	e1->add_component<TransformComponent>(float4x4{ 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 });

	e1->update();
	e1->render();

	delete e1;
	return 0;
}