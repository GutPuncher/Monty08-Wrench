#include "Ruleset.h"

#include "Util.h"
#include "ParseTree.h"

inline bool Assemble::Rule::PreprocessorDirective::BeginsWithHash(std::string_view word) const
{
	return word[0] == '#';
}

Assemble::ParseEvent Assemble::Rule::DEFINE::ParseLiteral(std::string_view lineLiteral)
{
	return ParseEvent();
}

Assemble::ParseEvent Assemble::Rule::ALIGN::ParseLiteral(std::string_view lineLiteral)
{
	ParseEvent pe{};

	if (BeginsWithHash(lineLiteral)) {
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