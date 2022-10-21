#include "../Header Files/Includes.hpp"
#include "../Header Files/MH/MinHook.h"

// -------------------------------------------------------------------------------------------------------------------

#define INVALID_MODULE(str, mod) \
{ \
	std::cout << str << std::endl; \
	Sleep(2000); \
    FreeLibraryAndExitThread(mod, -1); \
} \

// -------------------------------------------------------------------------------------------------------------------

void OnStart()
{
	unsigned short status = 0;

	FILE* f{};
	AllocConsole();
	freopen_s(&f, "CONOUT$", "w", stdout);
	SetConsoleTitleA("Steam");

	if (GetModuleHandleA("steam.exe") == nullptr)
	{
		INVALID_MODULE("Unable to locate: steam.exe; Ensure you load the module into the correct process", Utility::_module);
	}
	else
	{
		status += 1;
		std::cout << "[" << status << "/4] " << "Found steam.exe" << std::endl;
	}

	// Cannot find steamservice.dll
	// ~ Required for this to work.
	if (GetModuleHandleA("steamservice.dll") == nullptr)
	{
		INVALID_MODULE("Unable to locate: steamservice.dll; Ensure steam.exe is running as administrator and try again", Utility::_module);
	}
	else
	{
		status += 1;
		std::cout << "[" << status << "/4] " << "Found steamservice.dll" << std::endl;
	}

	// The function responsible for loading VAC modules.
	// xref: pModule->m_pModule == NULL
	uint8_t* pSignature = Utility::FindMemorySequence("steamservice.dll", "55 8B EC 83 EC ? 53 56 8B 75 ? 8B D9 83 7E");

	if (pSignature == nullptr)
	{
		std::cout << "Invalid pointer" << std::endl;
		FreeLibraryAndExitThread(Utility::_module, -1);
	}
	else
	{
		status += 1;
		std::cout << "[" << status << "/4] " << "Found pointer" << std::endl;
	}

	// Place a hook on the function that loads the VAC modules.
	MH_Initialize();
	MH_CreateHook(pSignature, &Hooks::LoadVACModule, reinterpret_cast<LPVOID*>(&Hooks::oLoadVACModule));
	MH_EnableHook(MH_ALL_HOOKS);

	status += 1;
	std::cout << "[" << status << "/4] " << "Hook placed on 0x" << reinterpret_cast<uintptr_t*>(pSignature) << "\n" << std::endl;
}

// -------------------------------------------------------------------------------------------------------------------

BOOL APIENTRY DllMain(HMODULE hModule, uintptr_t uAttachReason, LPVOID lpReserved)
{
	if (uAttachReason == DLL_PROCESS_ATTACH)
	{
		Utility::_module = hModule;

		DisableThreadLibraryCalls(hModule);

		Utility::DestroyPESection(Utility::_module);

		OnStart();
	}

	return TRUE;
}

// -------------------------------------------------------------------------------------------------------------------