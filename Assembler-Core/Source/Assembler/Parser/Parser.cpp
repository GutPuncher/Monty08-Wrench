#include "Parser.h"

#include "Ruleset.h"
#include "Util.h"
#include "InstructionSet.h"

#include <iostream>
#include <sstream>
#include <algorithm>

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

	if (!m_Source) {
		std::cout << "Assembler/Parser [Error]: cannot load file " + path << "\n";
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

	size_t lineCount = 0;
	std::string line;
	auto& ruleset = preprocess ? m_RulePreprocessRegistry : m_RuleRegistry;
	std::ifstream& stream = preprocess ? m_SourcePreprocess : m_Source;

	while (std::getline(stream, line, '\n')) {
		std::istringstream lineStream(line);
		std::string terminal;

		transform(line.begin(), line.end(), line.begin(), ::tolower);
		std::vector<std::string> words{ 3 };
		Util::split(Util::trim(std::string(line)), words, ' ');

		if (line.size() == 0 || words.size() == 0) continue;

		bool unknownID = true;
		for (SyntaxRule* rule : ruleset) {
			ParseEvent ev = rule->ParseLiteral(words);

			if (ev.triggered && ev) {
				m_InSection = ev.sectionInit != AlignDir::Section::NONE ? ev.sectionInit : m_InSection;
				unknownID = false;

				if (!MatchRuleSection(rule->AvailableInSection())) {
					std::cout << "The given keyword cannot be used inside the " + Util::mapStrSection(m_InSection) + " section.\n";
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
					keyword = "warning";
					break;
				case ParseEvent::Severty::MEDIUM:
					keyword = "medium severty error";
					break;
				case ParseEvent::Severty::CRITICAL:
					keyword = "error";
					abort = true;
				}

				std::cout << "A(n) " + keyword + " occured while parsing the file \"" + m_Path + "\"\n";
				std::cout << ev.errorStatus << '\n' << ev.errorStr << '\n';

				if (abort) {
					std::cout << "The assembly process had been aborted.\n";
					return false;
				}
			}
		}

		if (unknownID && !preprocess) {
			if (Util::trim(line)[0] != '#') {
				std::cout << "Unknown Identifier at line " << std::to_string(lineCount) << '\n';
			}
		}

		lineCount++;
	}

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