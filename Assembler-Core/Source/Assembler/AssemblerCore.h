#pragma once

#include "Parser/ParseTree.h"

namespace Assemble {
	class Assembler {
	public:
		Assembler() = default;
		~Assembler() = default;

		bool AssembleFromTree(const ParseTree* tree);

	private:
		size_t GetBssSize(const ParseTree* tree) const;
		size_t GetDataSize(const ParseTree* tree) const;

		std::string MapIndexToSection(int index) const;

		int CalcIntersectionLenght(int start0, int start1, int end0, int end1) const;

	private:
		size_t m_MaxProgAdr = 0x4000;
		size_t m_MaxProgSize = 0x3000;
	};

	struct SectionInfo {
		AlignDir::Section section;
		size_t size;
		int offset;
		bool doesExist;
	};
}