#pragma once
#include <vector>
#include <string>

struct FileNode {
    std::string name;
    bool isDir;
    std::vector<FileNode> children;
};
