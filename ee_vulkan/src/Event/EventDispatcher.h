#pragma once
#include "Event.h"
#include <functional>
namespace ev
{
	class EventDispathcer final
	{
	public:
		EventDispathcer(const Event& e):even(e)
		{
		}
		template<typename T>
		void Dispatch(std::function<void(const T&)> callback)
		{
			if (T::GetStaticEventType() == even.GetEventType())
				callback(*(T*)&even);
		}

		const Event& even;
	};
}