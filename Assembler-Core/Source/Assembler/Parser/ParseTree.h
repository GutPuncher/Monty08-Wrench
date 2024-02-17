#pragma once

#include <string>
#include <vector>
#include <memory>

namespace Assemble {
	struct PreprocessorDirective;
	struct AlignDir;
	struct DefineDir;

	struct ParseTree {
		std::vector<AlignDir> preproc_align;
		std::vector<DefineDir> preproc_define;
	};

	struct PreprocessorDirective {
		enum class Directive { ALIGN, DEFINE };

		Directive dir;
	};

	struct AlignDir : PreprocessorDirective {
		enum class Section { BSS, TEXT, DATA, ERRORTYPE };

		Directive dir = Directive::ALIGN;
		Section sec;
		char multipleOf;
	};

	struct DefineDir : PreprocessorDirective {
		Directive dir = Directive::DEFINE;
		std::string symbolOld;
		std::string symbolNew;
	};
}