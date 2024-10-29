#pragma once

#include <iostream>
#include <filesystem>
#include <fstream>

using namespace std;
namespace fs = std::filesystem;

void CreateFiles(const string& file);
void CopyFile(const string& source, const string& destination);
void CurrentPath();
void FileSize(const string& file);
void RemoveFile(const string& file);
void ResizeFile(const string& file, uintmax_t new_size);
