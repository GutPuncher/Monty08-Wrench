#pragma once

#include "SyntaxRule.h"

#include <string_view>

namespace Assemble::Rule {
	class PreprocessorDirective : public SyntaxRule {
	public:
		PreprocessorDirective(void* branch)
			: SyntaxRule(branch) {}
		~PreprocessorDirective() = default;

		virtual ParseEvent ParseLiteral(std::string_view lineLiteral) = 0;

	protected:
		inline bool BeginsWithHash(std::string_view word) const;
	};

	class ALIGN : public PreprocessorDirective {
	public:
		explicit ALIGN(void* branch)
			: PreprocessorDirective(branch) {
			literal = "align";
		}
		~ALIGN() = default;

		virtual ParseEvent ParseLiteral(std::string_view lineLiteral) override;
	};

	class DEFINE : public PreprocessorDirective {
	public:
		explicit DEFINE(void* branch)
			: PreprocessorDirective(branch) {
			literal = "define";
		}
		~DEFINE() = default;

		virtual ParseEvent ParseLiteral(std::string_view lineLiteral) override;
	};
}
