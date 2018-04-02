#pragma once
#include <graphics/Transformable.hpp>
#include <memory>

namespace px
{
	struct Transform
	{
		explicit Transform(std::unique_ptr<Transformable> transform) : transform(std::move(transform)) {};

		std::unique_ptr<Transformable> transform;
	};
}