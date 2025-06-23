#include "input.h"

Input::Input()
{
	BindKey("outer", sf::Keyboard::O);
	BindKey("hole", sf::Keyboard::H);
	BindKey("exit", sf::Keyboard::Escape);
	BindKey("play", sf::Keyboard::Enter);
	BindKey("start", sf::Keyboard::Space);

	ResetStates();
}

void Input::ProcessInput()
{
	CheckHeldKeys();
}

void Input::ResetStates()
{
	for (auto& pair : buttonStates)
	{
		pair.second.isPressed = false;
	}
}

void Input::BindKey(const std::string& action, sf::Keyboard::Key key)
{
	keyBindings[action] = key;
	buttonStates[action] = { false };
}

bool Input::IsPressed(const std::string& action) const
{
	auto it = buttonStates.find(action);
	if (it != buttonStates.end())
	{
		return it->second.isPressed;
	}
	return false;
}

void Input::UpdateButtonState(const std::string& action, bool isPressed)
{
	auto it = buttonStates.find(action);
	if (it != buttonStates.end())
	{
		it->second.isPressed = isPressed;
	}
}

void Input::CheckHeldKeys()
{
	for (const auto& pair : keyBindings)
	{
		const std::string& action = pair.first;
		sf::Keyboard::Key key = pair.second;
		UpdateButtonState(action, sf::Keyboard::isKeyPressed(key));
	}
}