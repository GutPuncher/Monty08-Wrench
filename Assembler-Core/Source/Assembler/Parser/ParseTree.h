#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <algorithm>

namespace Assemble {
	struct PreprocessorDirective {
		enum class Directive { ALIGN, DEFINE };

		Directive dir;
	};

	struct AlignDir : PreprocessorDirective {
		enum class Section { BSS, TEXT, DATA, ERRORTYPE, NONE, GLOBAL };

		Directive dir = Directive::ALIGN;
		Section sec;
		char multipleOf;
	};

	struct DefineDir : PreprocessorDirective {
		Directive dir = Directive::DEFINE;
		std::string alias;
		std::string symbol;
	};

	struct OrganizeDir {
		AlignDir::Section sec;
		unsigned long offset;
	};

	struct Bytestream {
		std::vector<char> bytestream;
		size_t lenght;
	};

	struct BytestreamRes {
		size_t lenght;
	};

	struct Operand {
		enum class Type { NONE = 0, NUMERIC, NUMERIC_WIDE, LABEL, REGISTER, PORT, SYSTEM_CONSTANT };
	};

	struct Op_Numeric : public Operand {
		Op_Numeric(unsigned char val) : value(val) {}

		unsigned char value;
	};

	struct Op_NumericWide : public Operand {
		Op_NumericWide(int val) : value(val) {}

		int value;
	};

	struct Op_Label : public Operand {
		Op_Label(const std::string& val) : label(val) {}
		std::string label;

		static bool dispatchLabel(std::string& word) {
			if (word[0] == '[' && word[word.size() - 1] == ']') {
				word = word.substr(1, word.size() - 2);
				return true;
			}
			return false;
		}
	};

	struct Op_Register : public Operand {
		enum class Register { R0 = 0, R1 = 1, NONE };

		Op_Register(Register val) : reg(val) {}

		Register reg;

		static Register mapStringReg(std::string& str) {
			transform(str.begin(), str.end(), str.begin(), ::tolower);

			if (str == "r0") return Register::R0;
			else if (str == "r1") return Register::R1;
			else return Register::NONE;
		}
	};

	struct Op_Port : public Operand {
		enum class Port { P0, P1, NONE };

		Op_Port(Port val) : p(val) {}

		Port p;

		static Port mapStringPort(std::string& str) {
			transform(str.begin(), str.end(), str.begin(), ::tolower);

			if (str == "p0") return Port::P0;
			else if (str == "p1") return Port::P1;
			else return Port::NONE;
		}
	};

	struct Op_SystemConstant : public Operand {
		Op_SystemConstant(bool val) : value(val) {}

		bool value;
	};

	struct Instruction {
		unsigned char opcode;
		std::pair<Operand::Type, Operand*> param0 = { Operand::Type::NONE, nullptr };
		std::pair<Operand::Type, Operand*> param1 = { Operand::Type::NONE, nullptr };
		std::pair<Operand::Type, Operand*> op0 = { Operand::Type::NONE, nullptr };
		std::pair<Operand::Type, Operand*> op1 = { Operand::Type::NONE, nullptr };

		void OnDestruction() {
			delete param0.second;
			delete param1.second;
			delete op0.second;
			delete op1.second;
		}
	};

	struct SectionData {
		std::vector<Bytestream> data;
	};

	struct SectionText {
		std::vector<Instruction> code;

		~SectionText() {
			std::for_each(code.begin(), code.end(), [](Instruction& instr) { instr.OnDestruction(); });
		}
	};

	struct SectionBSS {
		std::vector<BytestreamRes> dataRes;
	};

	struct LabelPointer {
		enum class DataType { INSTRUCTION, RESERVE_BYTE, DEFINE_BYTE };

		void* ptr;
		DataType type;
	};

	struct ParseTree {
		std::vector<AlignDir> preproc_align;
		std::vector<DefineDir> preproc_define;
		std::vector<OrganizeDir> organizes;

		SectionBSS sec_bss;
		SectionData sec_data;
		SectionText sec_text;

		std::map<std::string, LabelPointer> labels;
	};
}