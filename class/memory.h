#pragma once

#include <Windows.h>
#include <string>

class RBXMemory {
public:
    uintptr_t id;
    uintptr_t base_address;
    void* handle;

    RBXMemory(std::string_view process_name = "RobloxPlayerBeta.exe");

    RBXMemory(const RBXMemory&) = delete;
    RBXMemory& operator=(const RBXMemory&) = delete;

    ~RBXMemory();

    static RBXMemory& get_instance(std::string_view process_name = "RobloxPlayerBeta.exe") {
        static RBXMemory instance(process_name);
        return instance;
    }

    uintptr_t get_module_address(std::string_view module_name) const;
    uintptr_t get_base() const;

    std::string read_string(uintptr_t address) const;
    std::string f_read_string(uintptr_t address) const;

    template <typename T>
    T read_memory(uintptr_t address) const noexcept;

    template <typename T>
    void write_memory(uintptr_t address, T value) const noexcept;
};

template <typename T>
inline void RBXMemory::write_memory(uintptr_t address, T value) const noexcept
{
    ::WriteProcessMemory(this->handle, reinterpret_cast<void*>(address), &value, sizeof(T), NULL);
}

template <typename T>
inline T RBXMemory::read_memory(uintptr_t address) const noexcept
{
    T buffer = {};
    ::ReadProcessMemory(this->handle, reinterpret_cast<void*>(address), &buffer, sizeof(T), NULL);
    return buffer;
}