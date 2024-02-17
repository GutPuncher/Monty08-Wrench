#include "Util.h"

size_t Assemble::Util::split(const std::string& str, std::vector<std::string>& outlist, char sep)
{
	outlist.clear();
	std::string word;
	for (char c : str) {
		if (c != sep) word += c;
		else if (!word.empty()) {
			outlist.push_back(word);
			word.clear();
		}
	}

	if (!word.empty()) outlist.push_back(word);

	return outlist.size();
}

Assemble::AlignDir::Section Assemble::Util::mapSectionStr(const std::string& str)
{
	if (str == "TEXT" || str == ".text") return AlignDir::Section::TEXT;
	else if (str == "BSS" || str == ".bss") return AlignDir::Section::TEXT;
	else if (str == "DATA" || str == ".data") return AlignDir::Section::DATA;
	else return AlignDir::Section::ERRORTYPE;
}