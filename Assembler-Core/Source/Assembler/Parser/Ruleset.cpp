#include "Ruleset.h"

#include "Util.h"
#include "ParseTree.h"

#include <algorithm>

inline bool Assemble::Rule::PreprocessorDirective::BeginsWithHash(std::string_view word) const
{
	if (word.size() == 0) return false;
	return word[0] == '#';
}

Assemble::ParseEvent Assemble::Rule::DEFINE::ParseLiteral(std::string_view lineLiteral)
{
	ParseEvent pe{};

	if (BeginsWithHash(Util::trim(std::string(lineLiteral)))) {
		std::vector<std::string> words{ 3 };
		Util::split(std::string(lineLiteral), words, ' ');
		if (words[0].substr(1, words[0].size() - 1) == literal) {
			pe.triggered = true;
			DefineDir directive{};
			directive.alias = words[1];
			directive.symbol = words[2];

			std::vector<DefineDir>* targetObj = static_cast<std::vector<DefineDir>*>(branch);
			targetObj->push_back(directive);

			pe.succeed = true;
		}
	}

	return pe;
}

Assemble::ParseEvent Assemble::Rule::ALIGN::ParseLiteral(std::string_view lineLiteral)
{
	ParseEvent pe{};

	if (BeginsWithHash(Util::trim(std::string(lineLiteral)))) {
		std::vector<std::string> words{ 3 };
		Util::split(std::string(lineLiteral), words, ' ');
		if (words[0].substr(1, words[0].size() - 1) == literal) {
			pe.triggered = true;
			AlignDir directive{};
			directive.sec = Util::mapSectionStr(words[1]);
			directive.multipleOf = std::atoi(words[2].c_str());

			if (directive.multipleOf == 0) {
				pe.severty = ParseEvent::Severty::CRITICAL;
				pe.errorStr = "The alignment offset must be greater than one.";
				return pe;
			}
			if (directive.sec == AlignDir::Section::ERRORTYPE) {
				pe.severty = ParseEvent::Severty::CRITICAL;
				pe.errorStr = "The given segement cannot be mapped.";
				return pe;
			}

			std::vector<AlignDir>* targetObj = static_cast<std::vector<AlignDir>*>(branch);
			targetObj->push_back(directive);

			pe.succeed = true;
		}
	}

	return pe;
}

Assemble::ParseEvent Assemble::Rule::ORG::ParseLiteral(std::string_view lineLiteral)
{
	ParseEvent pe{};

	std::vector<std::string> words{ 3 };
	Util::split(std::string(lineLiteral), words, ' ');
	std::string substr = words[0];
	transform(substr.begin(), substr.end(), substr.begin(), ::tolower);
	if (substr == "org") {
		pe.triggered = true;
		OrganizeDir directive{};
		directive.sec = Util::mapSectionStr(words[1]);
		Util::ValueReturnObject<long> vro = Util::convertToDecimal<long>(words[2].c_str());
		if (!vro.isValid) {
			pe.severty = ParseEvent::Severty::CRITICAL;
			pe.errorStr = "The given postfix indicating a number system does not match the given pattern ('q' 'h', 'o', 'b', 'd')\n";
			return pe;
		}

		if (directive.sec == AlignDir::Section::ERRORTYPE) {
			pe.severty = ParseEvent::Severty::CRITICAL;
			pe.errorStr = "The given segement cannot be mapped.";
			return pe;
		}

		directive.offset = vro.value;

		std::vector<OrganizeDir>* targetObj = static_cast<std::vector<OrganizeDir>*>(branch);
		targetObj->push_back(directive);

		pe.succeed = true;
	}

	return pe;
}

Assemble::ParseEvent Assemble::Rule::Section::ParseLiteral(std::string_view lineLiteral)
{
	ParseEvent pe{};

	std::vector<std::string> words{ 2 };
	Util::split(std::string(lineLiteral), words, ' ');
	std::string substr = words[0];
	transform(substr.begin(), substr.end(), substr.begin(), ::tolower);
	if (substr == "section") {
		pe.triggered = true;

		AlignDir::Section sec = Util::mapSectionStr(words[1]);

		if (sec == AlignDir::Section::ERRORTYPE || sec == AlignDir::Section::NONE) {
			pe.errorStr = "The given section is not valid.\n";
			pe.severty = ParseEvent::Severty::CRITICAL;
			return pe;
		}

		pe.sectionInit = sec;
		pe.succeed = true;
	}

	return pe;
}

Assemble::ParseEvent Assemble::Rule::Label::ParseLiteral(std::string_view lineLiteral)
{
	ParseEvent pe{};
	std::string lineStr = Util::trim(std::string(lineLiteral));
	if (lineStr[0] == '_' && lineStr[lineStr.size() - 1] == ':') {
		pe.triggered = true;

		std::map<std::string, void*>* targetObj = static_cast<std::map<std::string, void*>*>(branch);
		const std::string& substr = lineStr.substr(1, lineStr.size() - 2);
		targetObj->emplace(substr, nullptr);
		pe.labelRef = substr;

		pe.succeed = true;
	}

	return pe;
}

Assemble::ParseEvent Assemble::Rule::DefineByte::ParseLiteral(std::string_view lineLiteral)
{
	ParseEvent pe{};

	std::vector<std::string> words;
	words.reserve(20);
	Util::split(std::string(Util::trim(std::string(lineLiteral))), words, ' ');	// Splits string in multiple words due to whitespaces
	words.shrink_to_fit();

	if (words[0] == "db") {
		pe.triggered = true;

		std::vector<Bytestream>* targetObj = static_cast<std::vector<Bytestream>*>(branch);
		targetObj->push_back({});
		Bytestream& stream = (*targetObj)[targetObj->size() - 1];

		for (int i = 1; i < words.size(); i++) {
			const std::string& word = words[i];
			if (word[0] == '"' && word[word.size() - 1] == '"') {
				for (int j = 1; j < word.size() - 1; j++) {
					stream.bytestream.push_back(word[j]);
				}
			}
			else {
				Util::ValueReturnObject vro = Util::convertToDecimal<char>(word);
				if (!vro) {
					pe.severty = ParseEvent::Severty::WARNING;
					pe.errorStr = "Cannot parse value that has been defined by db-directive. Assuming 0.";
					return pe;
				}
				stream.bytestream.push_back(vro.value);
			}
			stream.lenght = stream.bytestream.size();
		}

		pe.labelRefObj = (void*)&stream;
		pe.ptrType = LabelPointer::DataType::DEFINE_BYTE;
		pe.succeed = true;
	}

	return pe;
}

Assemble::ParseEvent Assemble::Rule::ReserveByte::ParseLiteral(std::string_view lineLiteral)
{
	ParseEvent pe{};

	std::vector<std::string> words;
	Util::split(std::string(Util::trim(std::string(lineLiteral))), words, ' ');

	if (words[0] == "resb") {
		pe.triggered = true;

		std::vector<BytestreamRes>* targetObj = static_cast<std::vector<BytestreamRes>*>(branch);
		targetObj->push_back({});

		Util::ValueReturnObject vro = Util::convertToDecimal<size_t>(words[1]);

		if (!vro) {
			pe.severty = ParseEvent::Severty::CRITICAL;
			pe.errorStr = "Cannot parse value that has been defined by db-directive.";
			return pe;
		}

		BytestreamRes& stream = (*targetObj)[targetObj->size() - 1];
		stream.lenght = vro.value;

		pe.labelRefObj = (void*)&stream;
		pe.ptrType = LabelPointer::DataType::RESERVE_BYTE;
		pe.succeed = true;
	}

	return pe;
}