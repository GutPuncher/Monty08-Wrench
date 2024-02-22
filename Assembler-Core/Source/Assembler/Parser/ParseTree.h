#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>

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
		std::vector<unsigned char> bytestream;
		size_t lenght;
	};

	struct BytestreamRes {
		size_t lenght;
	};

	struct Instruction {
	};

	struct SectionData {
		std::vector<Bytestream> data;
	};

	struct SectionText {
		std::vector<Instruction*> code;
	};

	struct SectionBSS {
		std::vector<BytestreamRes> dataRes;
	};

	struct LabelPointer {
		enum class DataType { INSTRUCTION, RESERVE_BYTE, DEFINE_BYTE };

		void* ptr;
		DataType type;
	};

	namespace {
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
}