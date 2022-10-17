#pragma once

#include <functional>
#include <vector>
#include <string>
#include <iostream>

namespace handle {
	class Handle
	{
	public:
		Handle() {}
		virtual ~Handle() {}

		virtual void OnUpdate(float deltaTime) {}
		virtual void OnRender() {}
	};
}