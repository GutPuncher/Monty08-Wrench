#pragma once

#include <unordered_map>
#include <string>
#include <algorithm>
#include <stdexcept>

namespace Assemble {
	struct InstructionInformation {
		unsigned char opcode;

		// complex sysconstind hassysconst sysconst p0 p1 o0 o1
		unsigned char operandBitfield;

		unsigned char cylces;

		bool isValid = true;

		size_t getWordCount() const {
			size_t count = 1;
			for (int i = 0; i < 4; ++i) {
				if (operandBitfield & (1 << i)) {
					count++;
				}
			}
			return operandBitfield & 0b1000'0000 ? count - 1 : count;
		}
	};

	inline std::unordered_map<std::string, InstructionInformation> instructions;

	InstructionInformation getInstructionData(std::string& instrStr);
	void SetupInstructionData();
}