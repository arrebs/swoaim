#include "config/app_paths.h"

#include <windows.h>

#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <system_error>

namespace AppPaths
{
namespace
{
std::filesystem::path executablePath()
{
    wchar_t buffer[MAX_PATH]{};
    const DWORD length = GetModuleFileNameW(nullptr, buffer, MAX_PATH);
    if (length == 0 || length >= MAX_PATH)
    {
        std::error_code error;
        return std::filesystem::current_path(error);
    }
    return std::filesystem::path(buffer);
}

const std::filesystem::path& cachedConfigDirectory()
{
    static const std::filesystem::path directory = [] {
        wchar_t buffer[MAX_PATH]{};
        const DWORD length = GetEnvironmentVariableW(L"APPDATA", buffer, MAX_PATH);
        if (length > 0 && length < MAX_PATH)
            return std::filesystem::path(buffer) / L"whozetAI";
        return executablePath().parent_path();
    }();
    return directory;
}
} // namespace

std::string executableDirectory()
{
    return executablePath().parent_path().u8string();
}

std::string configDirectory()
{
    return cachedConfigDirectory().u8string();
}

std::string defaultConfigFile()
{
    return (cachedConfigDirectory() / "config.ini").u8string();
}

void migrateLegacyConfig()
{
    std::error_code error;
    const std::filesystem::path destinationDirectory = cachedConfigDirectory();
    std::filesystem::create_directories(destinationDirectory, error);
    if (error)
    {
        std::cerr << "[Config] Cannot create config directory "
                  << destinationDirectory.u8string() << ": " << error.message() << std::endl;
        return;
    }

    const std::filesystem::path destinationFile = destinationDirectory / "config.ini";
    error.clear();
    if (std::filesystem::exists(destinationFile, error))
        return;

    const std::filesystem::path legacyFile = executablePath().parent_path() / "config.ini";
    error.clear();
    if (!std::filesystem::exists(legacyFile, error))
        return;

    std::filesystem::copy_file(legacyFile, destinationFile,
        std::filesystem::copy_options::overwrite_existing, error);
    if (error)
    {
        std::cerr << "[Config] Config migration failed: " << error.message() << std::endl;
        return;
    }

    std::cout << "[Config] Migrated legacy config: " << legacyFile.u8string()
              << " -> " << destinationFile.u8string() << std::endl;
}
} // namespace AppPaths
