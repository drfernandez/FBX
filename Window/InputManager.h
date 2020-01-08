#pragma once

// includes
#include "header.h"
#include <bitset>

class InputManager
{
public:
private:
	// proper singleton closure
	InputManager() = default;
	~InputManager() = default;
	InputManager(const InputManager& copy) = default;
	InputManager& operator=(const InputManager& copy) = default;

	// data members
	std::bitset<256>			m_Keyboard;
	std::bitset<15>				m_Mouse;
	POINT						m_MousePos;
	POINT						m_MousePosPrev;

	// private functions

public:
	// singleton access
	static InputManager* GetInstance(void);

	// public functions
	void Initialize(void);
	void Shutdown(void);

	inline void SetKeyboardButtonOn(int index) { m_Keyboard[index] = true; }
	inline void SetKeyboardButtonOff(int index) { m_Keyboard[index] = false; }
	inline void SetMouseButtonOn(int index) { m_Mouse[index] = true; }
	inline void SetMouseButtonOff(int index) { m_Mouse[index] = false; }

	inline void SetMousePosition(int x, int y) { m_MousePos.x = x, m_MousePos.y = y; }
	inline void SetMousePositionPrevious(int x, int y) { m_MousePosPrev.x = x, m_MousePosPrev.y = y; }

	inline bool GetKeyboardButton(int index) { return m_Keyboard[index]; }
	inline bool GetMouseButton(int index) { return m_Mouse[index]; }

	inline POINT GetMousePosition(void) { return m_MousePos; }
	inline POINT GetMousePositionPrevious(void) { return m_MousePosPrev; }


};
