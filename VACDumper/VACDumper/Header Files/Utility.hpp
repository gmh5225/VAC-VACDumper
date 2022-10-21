#pragma once

#include "Includes.hpp"

namespace Utility
{
	inline HMODULE _module = nullptr;

	std::uint8_t* FindMemorySequence(const char* moduleName, const char* signature);
	void DestroyPESection(HMODULE _module);
}