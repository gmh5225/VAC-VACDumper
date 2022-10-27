#pragma once

#include "Includes.hpp"

// -------------------------------------------------------------------------------------------------------------------

class Module
{
public:
	std::uint32_t unCRC32;
	HMODULE	_module;
	void* modulePointer;
	int(__stdcall* EntryPointFn)(std::uint32_t functionId, void const* parameters, std::uint32_t parametersSize, void* result, std::uint32_t* resultSize);
	std::int32_t lastResult;

	// Size of the region for the module in memory.
	std::uint32_t moduleSize;

	// Actual memory region address.
	void* rawModule;

	std::uint32_t unknown1C;
	std::uint32_t unknown20;
};

// -------------------------------------------------------------------------------------------------------------------

namespace Hooks
{
	bool __stdcall LoadVACModule(Module* _module, char flags);
	inline decltype(&LoadVACModule) oLoadVACModule;
}

// -------------------------------------------------------------------------------------------------------------------