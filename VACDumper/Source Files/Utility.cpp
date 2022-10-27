#include "../Header Files/Utility.hpp"

// -------------------------------------------------------------------------------------------------------------------

std::uint8_t* Utility::FindMemorySequence(const char* moduleName, const char* signature)
{
    const HMODULE _module = GetModuleHandleA(moduleName);

    if (!_module)
        return nullptr;

    static auto sequenceToBytes = [](const char* pSequence)
    {
        auto bytes = std::vector<int>{};
        auto start = const_cast<char*>(pSequence);
        auto end = const_cast<char*>(pSequence) + std::strlen(pSequence);

        for (auto current = start; current < end; current++)
        {
            if (*current == '?')
            {
                current++;

                if (*current == '?')
                    current++;

                bytes.push_back(-1);
            }
            else
            {
                bytes.push_back(std::strtoul(current, &current, 16));
            }
        }

        return bytes;
    };

    PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)_module;
    PIMAGE_NT_HEADERS ntHeaders = (PIMAGE_NT_HEADERS)((std::uint8_t*)(_module)+dosHeader->e_lfanew);

    DWORD moduleSize = ntHeaders->OptionalHeader.SizeOfImage;
    std::vector<int> sequenceBytes = sequenceToBytes(signature);
    uint8_t* scannedBytes = reinterpret_cast<std::uint8_t*>(_module);

    size_t size = sequenceBytes.size();
    int* content = sequenceBytes.data();

    for (unsigned long i = 0UL; i < moduleSize - size; ++i)
    {
        bool bFound = true;

        for (unsigned long j = 0UL; j < size; ++j)
        {
            if (scannedBytes[i + j] != content[j] && content[j] != -1)
            {
                bFound = false;
                break;
            }
        }

        if (bFound)
            return &scannedBytes[i];
    }

    // Invalid.
    return nullptr;
}

// -------------------------------------------------------------------------------------------------------------------

void Utility::DestroyPESection(HMODULE _module)
{
    if (!_module)
        return;

    // Initialize the DOS_HEADER structure by setting it to our DLL's module address.
    PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)_module;

    // Validity check because injectors can remove optional headers.
    if (!(dosHeader->e_magic == 0x4D5A || dosHeader->e_magic == 0x5A4D))
    {
        std::cout << "MZ header is invalid. Headers already tampered with?" << std::endl;
        return;
    }

    void* baseAddr = (void*)_module;
    DWORD oldProtection = 0x0;

    // Entire page.
    unsigned const short pageSize = 4096;

    VirtualProtect(baseAddr, pageSize, PAGE_EXECUTE_READWRITE, &oldProtection);
    std::memset(baseAddr, 0x0, pageSize);
    VirtualProtect(baseAddr, pageSize, oldProtection, &oldProtection);

    HANDLE processHandle = GetCurrentProcess();

    if (processHandle != INVALID_HANDLE_VALUE)
    {
        VirtualFreeEx(processHandle, (LPVOID)_module, 4096, MEM_DECOMMIT);
        CloseHandle(processHandle);
    }
}

// -------------------------------------------------------------------------------------------------------------------