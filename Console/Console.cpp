// Console.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <Windows.h>
#include <iostream>
#include <functional>


int main()
{
	using FuncPtr = void(*)(
		const char*,
		const char*,
		const char*,
		const char*
		);

	std::function<void(const char*, const char*, const char*, const char*)> func;

	HMODULE mod = LoadLibrary(L"FBXINOUT.dll");
	FuncPtr func_ptr = (FuncPtr)GetProcAddress(mod, "ExportFbxDataFromFile");

	if (func_ptr)
		func_ptr(
			"../Assets/monkeyface.fbx",
			"../Assets/monkeyface.msh",
			nullptr,
			nullptr
		);

	bool freed = FreeLibrary(mod);

	return 0;
}