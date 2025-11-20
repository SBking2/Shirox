#pragma once

namespace srx
{
	class Layer
	{
	public:
		Layer() = default;
		virtual ~Layer() = default;
		virtual void OnAttach() = 0;
		virtual void OnUpdate(float delta_time) = 0;
		virtual void OnDetach() = 0;
	};
}