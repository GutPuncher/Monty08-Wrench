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
		Parser();
		~Parser();

		bool LoadFromFile(const std::string path);
		bool Parse(bool preprocess = false);

	private:
		void SetupRules();

	private:
		ParseTree m_Tree;

		std::string m_Path;
		std::ifstream m_Source;

		std::vector<SyntaxRule*> m_RuleRegistry;
		std::vector<SyntaxRule*> m_RulePreprocessRegistry;
	};
}