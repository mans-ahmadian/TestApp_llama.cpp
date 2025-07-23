#pragma once
#include <string>
#include <filesystem>
#ifdef _WIN32
#include <windows.h>
#endif

inline std::string GetModelPath(std::filesystem::path modelName) {
    // Find the directory of the executable
    std::filesystem::path exe_dir;
#ifdef _WIN32
    char exe_path[MAX_PATH];
    if (GetModuleFileNameA(NULL, exe_path, MAX_PATH) > 0) {

        exe_dir = std::filesystem::path(exe_path).parent_path();
    }
#else
    char exe_path[4096];
    ssize_t len = readlink("/proc/self/exe", exe_path, sizeof(exe_path) - 1);
    if (len > 0) {
        exe_path[len] = '\0';
        exe_dir = std::filesystem::path(exe_path).parent_path().string();
    }
#endif

	auto model_path = exe_dir /"models" / modelName;
	return model_path.string();

}


