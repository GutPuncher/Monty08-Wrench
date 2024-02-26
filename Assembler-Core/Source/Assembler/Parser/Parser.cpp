#include "Parser.h"

#include "Ruleset.h"
#include "Util.h"
#include "InstructionSet.h"

#include <iostream>
#include <sstream>
#include <algorithm>

#include <fmt/core.h>
#include <fmt/color.h>

Assemble::Parser::Parser()
{
	SetupRules();
	SetupInstructionData();
}

Assemble::Parser::~Parser()
{
	m_RulePreprocessRegistry.clear();
	m_RuleRegistry.clear();
}

bool Assemble::Parser::LoadFromFile(const std::string path)
{
	m_Path = path;
	m_Source = std::ifstream(path);
	m_SourcePreprocess = std::ifstream(path);

	std::cout << "Loading file for parsing: " + path + '\n';

	if (!m_Source) {
		fmt::print(fg(fmt::color::crimson), "[Error]: Cannot load file {}\n", path);
		return false;
	}

	return true;
}

bool Assemble::Parser::Parse(bool preprocess)
{
	if (!m_Source) {
		std::cout << "You must load a file first.\n";
		return false;
	}

	std::cout << "Parsing file. Preprocessor run: " << (preprocess ? "true\n" : "false\n");
	std::cout << "----------------------------------------------\n";

	size_t lineCount = 0;
	std::string line;
	auto& ruleset = preprocess ? m_RulePreprocessRegistry : m_RuleRegistry;
	std::ifstream& stream = preprocess ? m_SourcePreprocess : m_Source;

	while (std::getline(stream, line, '\n')) {
		lineCount++;
		std::istringstream lineStream(line);
		std::string terminal;

		transform(line.begin(), line.end(), line.begin(), ::tolower);
		std::vector<std::string> words{ 5 };
		Util::split(Util::trim(std::string(line)), words, ' ');

		if (line.size() == 0 || words.size() == 0) continue;

		bool unknownID = true;
		for (SyntaxRule* rule : ruleset) {
			ParseEvent ev = rule->ParseLiteral(words);

			if (ev.triggered && ev) {
				m_InSection = ev.sectionInit != AlignDir::Section::NONE ? ev.sectionInit : m_InSection;
				unknownID = false;

				if (!MatchRuleSection(rule->AvailableInSection())) {
					std::cout << "  -> " + line + '\n';

					fmt::print(fg(fmt::color::crimson),
						"[Error]: Line {0}: The given keyword cannot be used inside the {1} section.\n", lineCount, Util::mapStrSection(m_InSection));

					fmt::print(fg(fmt::color::crimson),
						"  -> {}\n", line);
					return false;
				}

				if (m_ToBeLabeled) {
					m_Tree.labels[m_LabelRefKey].ptr = ev.labelRefObj;
					m_Tree.labels[m_LabelRefKey].type = ev.ptrType;
					m_ToBeLabeled = false;
				}
				else if (ev.labelRef.size() > 1) {
					m_LabelRefKey = ev.labelRef;
					m_ToBeLabeled = true;
				}

				break;
			}

			else if (ev.triggered && !ev) {
				std::string keyword;
				bool abort = false;

				switch (ev.severty)
				{
				case ParseEvent::Severty::WARNING:
					keyword = "[Warning]:";
					break;
				case ParseEvent::Severty::MEDIUM:
					keyword = "[Medium Severty Error]:";
					break;
				case ParseEvent::Severty::CRITICAL:
					keyword = "[Error]:";
					abort = true;
				}

				fmt::print(fg(fmt::color::crimson),
					"{0} Line {1}: {2}", keyword, std::to_string(lineCount), ev.errorStr);
				if (ev.errorStatus.size()) fmt::print(fg(fmt::color::crimson), " (Status: {})", ev.errorStatus);
				fmt::print(fg(fmt::color::crimson), "\n  -> {}\n", line);

				if (abort) {
					return false;
				}
			}
		}

		if (unknownID && !preprocess) {
			if (Util::trim(line)[0] != '#') {
				fmt::print(fg(fmt::color::orange), "Unknown Identifier at line {} will be ignored.\n", std::to_string(lineCount));
				fmt::print(fg(fmt::color::orange), "  -> {}\n", line);
			}
		}
	}

	fmt::print(fg(fmt::color::green), "=> Successfully parsed file {0} for Configuration: Preprocessor run: {1}", m_Path, (preprocess ? "true\n\n" : "false\n\n"));
	return true;
}

void Assemble::Parser::SetupRules()
{
	// Preprocessor
	m_RulePreprocessRegistry.push_back((SyntaxRule*)(Rule::PreprocessorDirective*) new Rule::ALIGN(&m_Tree.preproc_align));
	m_RulePreprocessRegistry.push_back((SyntaxRule*)(Rule::PreprocessorDirective*) new Rule::DEFINE(&m_Tree.preproc_define));

	// Parsing
	m_RuleRegistry.push_back((SyntaxRule*) new Rule::ORG(&m_Tree.organizes));
	m_RuleRegistry.push_back(new Rule::Section(nullptr));
	m_RuleRegistry.push_back(new Rule::Label(&m_Tree.labels));
	m_RuleRegistry.push_back(new Rule::DefineByte(&m_Tree.sec_data.data));
	m_RuleRegistry.push_back(new Rule::ReserveByte(&m_Tree.sec_bss.dataRes));
	m_RuleRegistry.push_back(new Rule::InstructionDir(&m_Tree.sec_text.code));
}

inline bool Assemble::Parser::MatchRuleSection(const std::vector<AlignDir::Section>& sections) const
{
	if (std::find(sections.begin(), sections.end(), AlignDir::Section::GLOBAL) != sections.end()) return true;

	for (AlignDir::Section section : sections) {
		if (section == m_InSection) return true;
	}

	return false;
}