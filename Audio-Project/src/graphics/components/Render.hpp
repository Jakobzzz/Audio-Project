#pragma once
#include <memory>
#include <graphics/Renderable.hpp>

namespace px
{
	struct Render
	{
		explicit Render(std::unique_ptr<Renderable> object) : object(std::move(object)) {}

		std::unique_ptr<Renderable> object;
	};
}