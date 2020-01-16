#pragma once
#include <array>
#include <string>

class Component
{
private:
	
public:
	Component() { }
	virtual ~Component() { }
	virtual void update() { }
	virtual void render() { }

};

using float4 = std::array<float, 4>;
using float4x4 = std::array<float4, 4>;

class HealthComponent : public Component
{
private:
	int current_health = 100;
	int max_health = 100;

public:
	HealthComponent() = default;
	HealthComponent(int chp, int mhp) : current_health(chp), max_health(mhp) { }
	~HealthComponent() override { }

	void update() { current_health -= 1; }
	void render() { }


	inline int get_current_health() { return current_health; }
	inline int get_max_health() { return max_health; }
	inline void set_current_health(int hp) { current_health = hp; }
	inline void set_max_health(int hp) { max_health = hp; }
};

class NameComponent : public Component
{
private:
	std::string name;

public:
	NameComponent() = default;
	NameComponent(std::string n) : name(n) { }
	~NameComponent() override { }

	void update() { }
	void render() { }

	inline std::string get_name() { return name; }
	inline void set_name(std::string n) { name = n; }
};

class TransformComponent : public Component
{
private:
	float4x4 matrix = { 0 };

public:
	TransformComponent() = default;
	TransformComponent(float4x4 m) : matrix(m) { }
	~TransformComponent() override { }

	void update() { matrix[3][0] += 0.01f; matrix[3][1] += 0.01f; matrix[3][2] += 0.01f; }
	void render() { }

	inline float4x4 get_matrix() { return matrix; }
	inline void set_matrix(float4x4 m) { matrix = m; }

};