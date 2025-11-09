#pragma once
#include "Camera.h"
namespace srx
{
	class CameraController final
	{
	public:
		CameraController(Camera* camera);
		void OnEvent(const Event& e);
		void OnUpdate(float delta);
	private:
		void OnKeyEvent(const KeyEvent& e);
		void OnMouseButtonEvent(const MouseButtonEvent& e);
		void OnMouseMoveEvent(const MouseMoveEvent& e);
	public:
		float move_sensity = 50.0f;
		float rotation_sensity = 10.0f;
		float translation_sensity = 10.0f;
	private:
		Camera* _camera;

		glm::vec2 _move_input = glm::vec2(0.0f);
		glm::vec2 _mouse_delta = glm::vec2(0.0f);

		bool _is_fly_camera = false;	//是否进入自由视角模式
		bool _is_translation_camera = false;	//是否进入平移视角模式
	};
}