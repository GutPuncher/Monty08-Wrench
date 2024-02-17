#pragma once

#include <string>
#include <string_view>

namespace Assemble {
	struct ParseEvent;

	class SyntaxRule
	{
	public:
		SyntaxRule(std::string_view literal, void* branch)
			: literal(literal), branch(branch) {};
		explicit SyntaxRule(void* branch)
			: branch(branch) {};
		~SyntaxRule() = default;

		virtual ParseEvent ParseLiteral(std::string_view lineLiteral) = 0;

		inline std::string_view GetLiteral() const { return literal; }
		inline void SetLiteral(const std::string& literal) { this->literal = literal; }

		inline const void* GetBranch() { return branch; }
		inline void SetBranch(void* branch) { this->branch = branch; }

	protected:
		std::string literal;
		void* branch;
	};

	struct ParseEvent {
	public:
		ParseEvent() = default;
		~ParseEvent() = default;

	public:
		enum class Severty { OK = 0, WARNING, MEDIUM, CRITICAL };

		bool triggered = false;
		bool succeed = false;
		std::string_view errorStatus;
		std::string_view errorStr;
		Severty severty;

		explicit operator bool() const {
			return succeed;
		}
	};
}
