#pragma once
#include <cstdint>
namespace ev
{
#define GetStaticType(x) static EventType GetStaticEventType() { return x; }
#define GetType(x) EventType GetEventType() const override { return x; }

	enum EventType
	{
		WindowResize,
		Key,
		MouseButton,
		MouseMove
	};

	class Event
	{
	public:
		virtual EventType GetEventType() const { return WindowResize; };
	};

	class WindowResizeEvent : public Event
	{
	public:
		uint32_t width;
		uint32_t height;
		WindowResizeEvent(uint32_t width, uint32_t height) :width(width), height(height) {  }
		GetType(EventType::WindowResize)
		GetStaticType(EventType::WindowResize)
	};

	class KeyEvent : public Event
	{
	public:
		int keycode;
		int scancode;
		int action;
		int mods;
		KeyEvent(int keycode, int scancode, int action, int mods)
			: keycode(keycode), scancode(scancode), action(action), mods(mods) {  }
		
		GetType(EventType::Key)
		GetStaticType(EventType::Key)
	};

	class MouseButtonEvent : public Event
	{
	public:
		int keycode;
		int action;
		int mods;
		MouseButtonEvent(int keycode, int action, int mods)
			: keycode(keycode), action(action), mods(mods) {}
		GetType(EventType::MouseButton)
		GetStaticType(EventType::MouseButton)
	};

	class MouseMoveEvent : public Event
	{
	public:
		double x_pos;
		double y_pos;
		float x_delta;
		float y_delta;
		MouseMoveEvent(double x, double y, float x_delta, float y_delta) 
			: x_pos(x), y_pos(y), x_delta(x_delta), y_delta(y_delta) {  }
		GetType(EventType::MouseMove)
		GetStaticType(EventType::MouseMove)
	};
}