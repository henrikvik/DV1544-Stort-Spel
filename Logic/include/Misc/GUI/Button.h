#ifndef BUTTON_H
#define BUTTON_H

#pragma region Comment

/*
Class: Button
Author: Andreas Eriksson

Description:
This class creates buttons.

*/
#pragma endregion Description of class
#include <d3d11.h>
#include <SimpleMath.h>
#include "Mouse.h"
namespace Logic
{
	class Button
	{
	private:
		DirectX::SimpleMath::Rectangle rek;
		DirectX::SimpleMath::Vector2 m_texCoordStart;
		DirectX::SimpleMath::Vector2 m_texCoordEnd;
		std::string m_texture;
		bool pressed;
	public:
		Button();
		~Button();
		void initialize(DirectX::SimpleMath::Vector2 pos, DirectX::SimpleMath::Vector2 texCoordStart, DirectX::SimpleMath::Vector2 texCoordEnd, float height, float width, std::string texture);
		void update(DirectX::Mouse::State Mouse);
	};
}
#endif