#pragma once

#include "SyntaxRule.h"
#include "ParseTree.h"

#include <string>
#include <fstream>
#include <vector>

namespace Assemble {
	class Parser
	{
	public:
		Parser(bool microcode = true);
		~Parser();

		bool LoadFromFile(const std::string path);
		bool Parse(bool preprocess = false);

		inline ParseTree* getTree() { return &m_Tree; }

	private:
		void SetupRules();
		inline bool MatchRuleSection(const std::vector<AlignDir::Section>& sections) const;

	private:
		ParseTree m_Tree;

		std::string m_Path;
		std::ifstream m_Source;

		AlignDir::Section m_InSection = AlignDir::Section::NONE;
		std::string m_LabelRefKey;
		bool m_ToBeLabeled = false;

		std::vector<SyntaxRule*> m_RuleRegistry;
		std::vector<SyntaxRule*> m_RulePreprocessRegistry;
	};
}