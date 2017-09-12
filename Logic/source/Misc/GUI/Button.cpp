#include "..\..\..\include\Misc\GUI\Button.h"
#include <iostream>

Logic::Button::Button()
{
}

Logic::Button::~Button()
{
}

void Logic::Button::initialize(DirectX::SimpleMath::Vector2 pos, DirectX::SimpleMath::Vector2 texCoordStart, DirectX::SimpleMath::Vector2 texCoordEnd, float height, float width, std::string texture)
{
	rek = DirectX::SimpleMath::Rectangle(pos.x, pos.y, width, height);
	m_texCoordStart = texCoordStart;
	m_texCoordEnd = texCoordEnd;
	m_texture = texture;
	pressed = false;
}

void Logic::Button::update(DirectX::Mouse::State Mouse)
{
	if (rek.Contains(Mouse.x, Mouse.y))
	{
		if (Mouse.leftButton && !pressed)
		{
			std::cout << "Left Trigger";
			pressed = true;
		}
		else if (!Mouse.leftButton && pressed)
		{
			pressed = false;
		}
		else
		{

		}
	}
}
