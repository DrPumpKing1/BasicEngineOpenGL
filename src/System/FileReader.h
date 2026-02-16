#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class FileReader
{
public:
    static std::string ReadFile(const std::string &filePath);
};