#pragma once

#include "ParseTree.h"

#include <vector>
#include <string>

namespace Assemble::Util {
	size_t split(const std::string& str, std::vector<std::string>& outlist, char sep);
	AlignDir::Section mapSectionStr(const std::string& str);
}