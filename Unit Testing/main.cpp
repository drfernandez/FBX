#include <Windows.h>
#include "Entity.h"


int main(int argc, char* argv[]) 
{
	Entity* e1 = new Entity();
	e1->add_component<TransformComponent>();
	e1->get_component<TransformComponent>().set_matrix({ 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 });
	

	delete e1;
	return 0;
}