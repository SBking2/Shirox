#include "CameraController.h"
#include "Core/Utils/Utils.h"
#include "Core/Event/EVCode/EVKeyCode.h"
#include "Core/Event/EVCode/EVMouseButtonCode.h"
#include "Core/Window/Window.h"
#include <GLM/gtc/matrix_transform.hpp>
namespace ev
{
	CameraController::CameraController(Camera* camera):_camera(camera)
	{
	}
	void CameraController::OnEvent(const Event& e)
	{
		EventDispathcer dispatcher(e);
		dispatcher.Dispatch<KeyEvent>(std::bind(&CameraController::OnKeyEvent, this, std::placeholders::_1));
		dispatcher.Dispatch<MouseButtonEvent>(std::bind(&CameraController::OnMouseButtonEvent, this, std::placeholders::_1));
		dispatcher.Dispatch<MouseMoveEvent>(std::bind(&CameraController::OnMouseMoveEvent, this, std::placeholders::_1));
	}

	void CameraController::OnUpdate(float delta)
	{
		if (_is_fly_camera)
		{
			//相机移动逻辑
			glm::vec3 move_direct = glm::rotate(_camera->rotation, glm::vec3(-_move_input.x, 0.0f, _move_input.y));
			_camera->position -= move_direct * move_sensity * delta;	//因为camera看向-Z，所以所有方向都是反的

			//相机旋转
			float yaw_delta = -_mouse_delta.x * rotation_sensity * delta;
			float pitch_delta = -_mouse_delta.y * rotation_sensity * delta;

			_mouse_delta = glm::vec2(0.0f);

			glm::vec3 right = glm::normalize(glm::rotate(_camera->rotation, glm::vec3(1.0f, 0.0f, 0.0f)));
			glm::quat pitch_quat = glm::angleAxis(glm::radians(pitch_delta), right);
			glm::quat yaw_quat = glm::angleAxis(glm::radians(yaw_delta), glm::vec3(0.0f, 1.0f, 0.0f));
			_camera->rotation = yaw_quat * pitch_quat * _camera->rotation;
		}

		if (_is_translation_camera)
		{
			glm::vec3 right = glm::normalize(glm::rotate(_camera->rotation, glm::vec3(1.0f, 0.0f, 0.0f)));
			glm::vec3 up = glm::normalize(glm::rotate(_camera->rotation, glm::vec3(0.0f, 1.0f, 0.0f)));

			glm::vec3 move_direct = -_mouse_delta.x * right * translation_sensity * delta;
			move_direct += _mouse_delta.y * up * translation_sensity * delta;

			_mouse_delta = glm::vec2(0.0f);

			_camera->position += move_direct;
		}
	}

	void CameraController::OnKeyEvent(const KeyEvent& e)
	{	
		if (e.action != EV_RELEASE)
		{
			switch (e.keycode)
			{
			case EV_KEY_W:
				_move_input.y = 1.0f;
				break;
			case EV_KEY_S:
				_move_input.y = -1.0f;
				break;
			case EV_KEY_A:
				_move_input.x = -1.0f;
				break;
			case EV_KEY_D:
				_move_input.x = 1.0f;
				break;
			}
		}else if (e.action == EV_RELEASE)
		{
			switch (e.keycode)
			{
			case EV_KEY_W:
				_move_input.y = 0.0f;
				break;
			case EV_KEY_S:
				_move_input.y = 0.0f;
				break;
			case EV_KEY_A:
				_move_input.x = 0.0f;
				break;
			case EV_KEY_D:
				_move_input.x = 0.0f;
				break;
			}
		}
	}

	void CameraController::OnMouseButtonEvent(const MouseButtonEvent& e)
	{
		if (e.action == EV_PRESS)
		{
			switch (e.keycode)
			{
			case EV_MOUSE_BUTTON_RIGHT:
				if (!_is_translation_camera)
				{
					_is_fly_camera = true;
					Window::GetInstance()->SetCursorLockMode(true);
				}
				break;
			case EV_MOUSE_BUTTON_MIDDLE:
				if (!_is_fly_camera)
				{
					_is_translation_camera = true;
					Window::GetInstance()->SetCursorLockMode(true);
				}
				break;
			}
		}

		if (e.action == EV_RELEASE)
		{
			switch (e.keycode)
			{
			case EV_MOUSE_BUTTON_RIGHT:
				if (!_is_translation_camera)
				{
					_is_fly_camera = false;
					Window::GetInstance()->SetCursorLockMode(false);
				}
				break;
			case EV_MOUSE_BUTTON_MIDDLE:
				if (!_is_fly_camera)
				{
					_is_translation_camera = false;
					Window::GetInstance()->SetCursorLockMode(false);
				}
				break;
			}
		}
	}

	void CameraController::OnMouseMoveEvent(const MouseMoveEvent& e)
	{
		if (!_is_fly_camera && !_is_translation_camera)
			_mouse_delta = glm::vec2(0.0f, 0.0f);
		else
			_mouse_delta = glm::vec2(e.x_delta, e.y_delta);
	}
}