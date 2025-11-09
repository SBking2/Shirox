#pragma once
#include <memory>
#include <utility>
namespace srx
{
	template<typename T>
	using Ref = std::shared_ptr<T>;

	template<typename T, typename... Args>
	std::shared_ptr<T> CreateRef(Args&&... args) {
		return std::make_shared<T>(std::forward<Args>(args)...);
	}
}