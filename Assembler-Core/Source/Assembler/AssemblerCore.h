#pragma once

#include "Parser/ParseTree.h"

#include <string>
#include <fstream>

namespace Assemble {
	class Assembler {
	public:
		Assembler() = default;
		~Assembler() = default;

		bool AssembleFromTree(const ParseTree* tree, const std::string& outFile);

	private:
		static size_t GetBssSize(const ParseTree* tree);
		static size_t GetDataSize(const ParseTree* tree);

		std::string MapIndexToSection(int index) const;

		int CalcIntersectionLenght(int start0, int start1, int end0, int end1) const;

	private:
		size_t m_MaxProgAdr = 0x0100;
		size_t m_MaxProgSize = 0x0020;

		std::ofstream m_ExecutableHandle;

		static bool generateText(const ParseTree* tree, std::ofstream& stream);
		static bool generateBss(const ParseTree* tree, std::ofstream& stream);
		static bool generateData(const ParseTree* tree, std::ofstream& stream);
	};

	struct SectionInfo {
		AlignDir::Section section;
		size_t size;
		int offset;
		bool doesExist;

		bool (*generate)(const ParseTree* tree, std::ofstream& stream);

		bool operator< (const SectionInfo& other) const {
			return offset < other.offset;
		}
	};
}