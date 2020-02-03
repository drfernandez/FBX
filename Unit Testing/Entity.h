#pragma once

#include <map>
#include <type_traits>  // decay
#include <memory>	    // unique_ptr

#include "Component.h"

struct Counter
{
	static int c;
};

int Counter::c = 0;

template <typename T>
struct ClassID : Counter
{
	static int id() noexcept
	{
		static int c = Counter::c++;
		return c;
	}
};

template <typename T>
int ClassID_v = ClassID<std::decay<T>>::id();

class Entity
{
private:
	std::map<int, std::unique_ptr<Component>> components;

public:
    Entity()
    {
        components.clear();
    }

    ~Entity()
    {
        components.clear();
    }

    Entity(const Entity& that) = delete;
    Entity& operator=(const Entity& that) = delete;

    template <typename T>
    T& get_component()
    {
        return static_cast<T&>(*components[ClassID_v<T>]);
    }

    template <typename T>
    bool has_component() 
    { 
        return components.count(ClassID_v<T>); 
    }

    template <typename T, typename... Args>
    Entity& add_component(Args&&... args)
    {
        components.emplace(ClassID_v<T>, std::make_unique<T>(std::forward<Args>(args)...));
        return *this;
    }

    void update()
    {
        for (auto& c : components)
        {
            c.second->update();
        }
    }

    void render()
    {
        for (auto& c : components)
        {
            c.second->render();
        }
    }

};
