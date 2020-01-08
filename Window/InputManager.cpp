#include "InputManager.h"

InputManager * InputManager::GetInstance(void)
{
	static InputManager instance;
	return &instance;
}

void InputManager::Initialize(void)
{
	memset(&m_Keyboard, 0, sizeof(m_Keyboard));
	memset(&m_Mouse, 0, sizeof(m_Mouse));
}

void InputManager::Shutdown(void)
{
}
