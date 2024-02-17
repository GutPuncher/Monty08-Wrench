#include "Parser.h"

#include "Ruleset.h"

#include <iostream>
#include <sstream>

Assemble::Parser::Parser()
{
	SetupRules();
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
	while (std::getline(m_Source, line, '\n')) {
		std::istringstream lineStream(line);
		std::string terminal;

		bool unknownID = true;
		for (SyntaxRule* rule : ruleset) {
			ParseEvent ev = rule->ParseLiteral(line);

			if (ev.triggered && ev) {
				unknownID = false;
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

		if (unknownID) {
			if (line[0] == '#') {
				std::cout << "Unknown Identifier at line " << std::to_string(lineCount) << '\n';
			}
		}

		lineCount++;
	}

	return false;
}

void Assemble::Parser::SetupRules()
{
	m_RulePreprocessRegistry.push_back((SyntaxRule*)(Rule::PreprocessorDirective*) new Rule::ALIGN(&m_Tree.preproc_align));
	m_RulePreprocessRegistry.push_back((SyntaxRule*)(Rule::PreprocessorDirective*) new Rule::DEFINE(&m_Tree.preproc_define));
}