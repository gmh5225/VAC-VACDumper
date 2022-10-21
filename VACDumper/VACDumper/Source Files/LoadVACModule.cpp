#include "../Header Files/Includes.hpp"

// -------------------------------------------------------------------------------------------------------------------

#define NATIVE_DIRECTORY "C:\\Modules"
#define MODULE_DIRECTORY "C:\\Modules\\0x%p.dll"
#define CODE_DIRECTORY	 "C:\\Modules\\0x%p.bin"

// -------------------------------------------------------------------------------------------------------------------

bool __stdcall Hooks::LoadVACModule(Module* _module, char flags)
{
	constexpr static const char* directory = NATIVE_DIRECTORY;
	bool directoryExists                   = std::filesystem::is_directory(directory);

	// Create a directory to store the dumps in.
	if (!directoryExists)
		CreateDirectory(directory, NULL);

	if (_module->rawModule)
	{
		char buffer[999];
		const PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)_module->rawModule;
		bool validPE                      = dosHeader->e_magic == 0x4D5A || dosHeader->e_magic == 0x5A4D;
		std::string dumpName              = "";

		validPE ? dumpName = MODULE_DIRECTORY
				: dumpName = CODE_DIRECTORY;

		sprintf_s(buffer, dumpName.c_str(), _module->rawModule);

		const HANDLE fileHandle = CreateFileA(buffer, GENERIC_READ | GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);

		if (fileHandle != INVALID_HANDLE_VALUE)
		{
			if (_module->unCRC32 && _module->moduleSize >= 0x1000)
			{
				// Write the file to disk.
				if (WriteFile(fileHandle, _module->rawModule, _module->moduleSize, nullptr, nullptr))
				{
					// Alert when this hook is called and we should dump a module.
					Beep(400, 400);

					// Manual mapped flag.
					if (flags & 2)
					{
						validPE ? std::cout << "Dumped mapped module" << " (" << buffer << ")" << std::endl
								: std::cout << "Dumped memory"		  << " (" << buffer << ")" << std::endl;
					}
				}

				// Delete the file handle.
				CloseHandle(fileHandle);
			}
			else
			{
				std::cout << "Invalid file handle" << std::endl;
			}
		}
	}

	return oLoadVACModule(_module, flags);
}

// -------------------------------------------------------------------------------------------------------------------