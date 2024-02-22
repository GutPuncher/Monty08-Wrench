#pragma once

#include "SyntaxRule.h"

#include <string_view>

namespace Assemble::Rule {
	class PreprocessorDirective : public SyntaxRule {
	public:
		PreprocessorDirective(void* branch)
			: SyntaxRule(branch) {
			partOfSection = { AlignDir::Section::GLOBAL };
		}
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

	class ORG : public SyntaxRule {
	public:
		ORG(void* branch)
			: SyntaxRule(branch) {
			partOfSection = { AlignDir::Section::GLOBAL };
		}
		~ORG() = default;

		virtual ParseEvent ParseLiteral(std::string_view lineLiteral) override;
	};

	class Section : public SyntaxRule {
	public:
		explicit Section(void* branch)
			: SyntaxRule(branch) {
			partOfSection = { AlignDir::Section::GLOBAL };
		}
		~Section() = default;

		virtual ParseEvent ParseLiteral(std::string_view lineLiteral) override;
	};

	class Label : public SyntaxRule {
	public:
		explicit Label(void* branch)
			: SyntaxRule(branch) {
			partOfSection = { AlignDir::Section::BSS, AlignDir::Section::TEXT, AlignDir::Section::DATA };
		}
		~Label() = default;

		virtual ParseEvent ParseLiteral(std::string_view lineLiteral) override;
	};

	class DefineByte : public SyntaxRule {
	public:
		explicit DefineByte(void* branch)
			: SyntaxRule(branch) {
			partOfSection = { AlignDir::Section::DATA };
		}
		~DefineByte() = default;

		virtual ParseEvent ParseLiteral(std::string_view lineLiteral) override;
	};

	class ReserveByte : public SyntaxRule {
	public:
		explicit ReserveByte(void* branch)
			: SyntaxRule(branch) {
			partOfSection = { AlignDir::Section::BSS };
		}
		~ReserveByte() = default;

		virtual ParseEvent ParseLiteral(std::string_view lineLiteral) override;
	};
}
