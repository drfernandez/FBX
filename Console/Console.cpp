// Console.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <Windows.h>
#include <iostream>
#include <functional>


int main()
{
	using FuncPtr = void(*)(const char*, const char*, const char*, const char*,	bool, bool);
	std::function<void(const char*, const char*, const char*, const char*, bool, bool)> func;

	HMODULE mod = LoadLibrary(L"FBXINOUT.dll");
	FuncPtr func_ptr = (FuncPtr)GetProcAddress(mod, "ExportFbxDataFromFile");

	if (func_ptr)
		func_ptr(
			"../Assets/floor.fbx",
			"../Assets/floor.msh",
			"../Assets/floor.pose",
			"../Assets/floor.anim",
			true,
			true
		);

	bool isModuleFree = FreeLibrary(mod);

	return 0;
}