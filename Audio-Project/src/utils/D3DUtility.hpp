/*
	Utility class for D3D functionality
*/

#pragma once
#include <string>
#include <Windows.h>

namespace px
{
	//Release COM-object
	#define SAFE_RELEASE(arg) if(*arg != nullptr) { (*arg)->Release(); (*arg) = nullptr; } 
	#define WIDTH 1280U
	#define HEIGHT 800U

	enum ShaderType
	{
		VS = 0,
		PS = 0x1
	};

	DEFINE_ENUM_FLAG_OPERATORS(ShaderType);

	inline std::wstring ToWChar(const std::string & inputString)
	{
		return std::wstring(inputString.begin(), inputString.end());
	}
}