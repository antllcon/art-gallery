#ifndef INPUT_H
#define INPUT_H

#include <SFML/Window.hpp>
#include <string>
#include <unordered_map>

class Input
{
public:
	Input();
	void ProcessInput();
	void ResetStates();
	void BindKey(const std::string& action, sf::Keyboard::Key key);
	bool IsPressed(const std::string& action) const;

private:
	struct ButtonState
	{
		bool isPressed = false;
	};

	std::unordered_map<std::string, ButtonState> buttonStates;
	std::unordered_map<std::string, sf::Keyboard::Key> keyBindings;

	void CheckHeldKeys();
	void UpdateButtonState(const std::string& action, bool isPressed);
};

#endif