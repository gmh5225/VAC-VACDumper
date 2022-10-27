#include "../Header Files/Includes.hpp"

// -------------------------------------------------------------------------------------------------------------------

#define NATIVE_DIRECTORY "C:\\Modules"
#define MODULE_DIRECTORY "C:\\Modules\\0x%p.dll"
#define CODE_DIRECTORY	 "C:\\Modules\\0x%p.bin"

// -------------------------------------------------------------------------------------------------------------------

bool __stdcall Hooks::LoadVACModule(Module* _module, char flags)
{
	static constexpr const char* directory = NATIVE_DIRECTORY;
	const bool directoryExists = std::filesystem::is_directory(directory);

	// Create a directory to store the dumps in.
	if (!directoryExists)
	{
		CreateDirectory(directory, NULL);
	}

	if (_module->rawModule != nullptr)
	{
		char buffer[999];
		const PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)_module->rawModule;
		const bool validPE = dosHeader->e_magic == 0x4D5A || dosHeader->e_magic == 0x5A4D;
		std::string dumpName = "";

		validPE ? dumpName = MODULE_DIRECTORY : dumpName = CODE_DIRECTORY;

		sprintf_s(buffer, dumpName.c_str(), _module->rawModule);

		const HANDLE hFile = CreateFileA(buffer, GENERIC_READ | GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);

		if (hFile != INVALID_HANDLE_VALUE)
		{
			if (_module->unCRC32 && _module->moduleSize >= 0x1000)
			{
				// Write the file to disk.
				if (WriteFile(hFile, _module->rawModule, _module->moduleSize, nullptr, nullptr))
				{
					// Alert for when we've dumped a module.
					Beep(400, 400);

					validPE ? std::cout << "Dumped mapped module" << " (" << buffer << ")" << std::endl
						: std::cout << "Dumped memory" << " (" << buffer << ")" << std::endl;
				}

				// Delete the file handle.
				CloseHandle(hFile);
			}
		}
		else
		{
			std::cout << "Invalid file handle (0x" << hFile << ")" << std::endl;
		}
	}

	return oLoadVACModule(_module, flags);
}

// -------------------------------------------------------------------------------------------------------------------