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
	else if (str == "BSS" || str == ".bss") return AlignDir::Section::BSS;
	else if (str == "DATA" || str == ".data") return AlignDir::Section::DATA;
	else return AlignDir::Section::ERRORTYPE;
}

std::string Assemble::Util::mapStrSection(AlignDir::Section section)
{
	switch (section) {
	case AlignDir::Section::BSS: return ".bss";
	case AlignDir::Section::DATA: return ".bss";
	case AlignDir::Section::ERRORTYPE: return "errortype";
	case AlignDir::Section::GLOBAL: return "global";
	case AlignDir::Section::TEXT: return ".text";
	case AlignDir::Section::NONE: return "none";
	default:
		return "[Cannot map section to string]";
	}
}

std::string Assemble::Util::trim(const std::string& str, const std::string& whitespace)
{
	const auto strBegin = str.find_first_not_of(whitespace);
	if (strBegin == std::string::npos)
		return ""; // no content

	const auto strEnd = str.find_last_not_of(whitespace);
	const auto strRange = strEnd - strBegin + 1;

	return str.substr(strBegin, strRange);
}

std::string Assemble::Util::reduce(const std::string& str, const std::string& fill, const std::string& whitespace)
{
	auto result = trim(str, whitespace);

	// replace sub ranges
	auto beginSpace = result.find_first_of(whitespace);
	while (beginSpace != std::string::npos)
	{
		const auto endSpace = result.find_first_not_of(whitespace, beginSpace);
		const auto range = endSpace - beginSpace;

		result.replace(beginSpace, range, fill);

		const auto newStart = beginSpace + fill.length();
		beginSpace = result.find_first_of(whitespace, newStart);
	}

	return result;
}