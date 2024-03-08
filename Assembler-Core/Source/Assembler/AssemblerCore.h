#pragma once

#include "Parser/ParseTree.h"

#include <string>
#include <fstream>
#include <unordered_map>

namespace Assemble {
	typedef std::vector<std::pair<size_t, std::string>> registry;
	typedef std::unordered_map<std::string, size_t> index;

	class Assembler {
	public:
		Assembler() = default;
		~Assembler() = default;

		bool AssembleFromTree(const ParseTree* tree, const std::string& outFile);
		inline void SetData(size_t progSize, size_t codeSize) { m_MaxProgAdr = progSize; m_MaxProgSize = codeSize; }
		void ToggleLogisimOut(bool val) { m_LogisimOut = val; }

		bool GenerateLogisim();

	private:
		struct BinaryOperand {
			bool isValid;
			bool wide;
			std::pair<char, char> binaries;

			explicit operator bool() const {
				return isValid;
			}
		};

	private:
		inline void CloseHandle(std::ofstream& handle);

		static size_t GetBssSize(const ParseTree* tree);
		static size_t GetDataSize(const ParseTree* tree);

		std::string MapIndexToSection(int index) const;

		int CalcIntersectionLenght(int start0, int start1, int end0, int end1) const;

		static bool generateText(const ParseTree* tree, std::ofstream& stream, size_t& bufPtr, index& ind, registry& reg);
		static bool generateBss(const ParseTree* tree, std::ofstream& stream, size_t& bufPtr, index& ind, registry& reg);
		static bool generateData(const ParseTree* tree, std::ofstream& stream, size_t& bufPtr, index& ind, registry& reg);

		static char matchParamValToBin(Operand* op, Operand::Type type, char index = 0);
		static BinaryOperand matchOpValToBin(Operand* op, Operand::Type type, registry& registry, size_t bufPtr);

		bool MatchLabels();

	private:
		size_t m_MaxProgAdr = 0x0100;
		size_t m_MaxProgSize = 0x0020;

		bool m_LogisimOut = false;
		std::string m_OutPath;

		size_t m_BufferPtr = 0;

		std::ofstream m_ExecutableHandle;

		index m_LabelIndex;
		registry m_LabelReg;
	};

	struct SectionInfo {
		AlignDir::Section section;
		size_t size;
		int offset;
		bool doesExist;

		bool (*generate)(const ParseTree* tree, std::ofstream& stream, size_t& bufPtr, index& ind, registry& reg);

		bool operator< (const SectionInfo& other) const {
			return offset < other.offset;
		}
	};
}