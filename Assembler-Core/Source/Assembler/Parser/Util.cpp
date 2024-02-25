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

std::pair<Assemble::Operand::Type, Assemble::Operand*> Assemble::Util::dispatchOperand(std::string& word)
{
	if (ValueReturnObject vro = convertToDecimal<int>(word)) {
		if (vro.value > 255) return { Operand::Type::NUMERIC_WIDE, (Operand*) new Op_NumericWide(vro.value) };
		else return { Operand::Type::NUMERIC, (Operand*) new Op_Numeric(static_cast<unsigned char>(vro.value)) };
	}
	else if (Op_Label::dispatchLabel(word)) {
		return { Operand::Type::LABEL, (Operand*) new Op_Label(word) };
	}
	else if (Op_Register::Register reg = Op_Register::mapStringReg(word); reg != Op_Register::Register::NONE) {
		return { Operand::Type::REGISTER, (Operand*) new Op_Register(reg) };
	}
	else if (Op_Port::Port port = Op_Port::mapStringPort(word); port != Op_Port::Port::NONE) {
		return { Operand::Type::PORT, (Operand*) new Op_Port(port) };
	}
	else {
		return { Operand::Type::NONE, nullptr };
	}

	return { Operand::Type::NONE, nullptr };
}