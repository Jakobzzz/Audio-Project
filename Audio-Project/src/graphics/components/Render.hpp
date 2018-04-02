#pragma once
#include <graphics/Renderable.hpp>
#include <memory>

namespace px
{
	struct Render
	{
		explicit Render(std::unique_ptr<Renderable> object) : object(std::move(object)) {}

		std::unique_ptr<Renderable> object;
	};
}