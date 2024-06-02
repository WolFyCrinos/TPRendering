#pragma once
#include <filesystem>

auto make_absolute_path(std::filesystem::path const& path) -> std::filesystem::path;
