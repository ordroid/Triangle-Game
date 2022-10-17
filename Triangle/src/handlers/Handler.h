#pragma once

#pragma once

#include <functional>
#include <vector>
#include <string>
#include <iostream>

namespace handler {
	class Handler
	{
	public:
		Handler() {}
		virtual ~Handler() {}

		virtual void OnUpdate(float deltaTime) {}
		virtual void OnRender() {}
	};
}