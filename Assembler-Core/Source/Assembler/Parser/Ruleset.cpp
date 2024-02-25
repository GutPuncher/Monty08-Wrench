#include "Ruleset.h"

#include "Util.h"
#include "ParseTree.h"
#include "InstructionSet.h"

inline bool Assemble::Rule::PreprocessorDirective::BeginsWithHash(std::string_view word) const
{
	if (word.size() == 0) return false;
	return word[0] == '#';
}

Assemble::ParseEvent Assemble::Rule::DEFINE::ParseLiteral(std::vector<std::string>& lineLiteral)
{
	ParseEvent pe{};

	if (BeginsWithHash(lineLiteral[0])) {
		if (lineLiteral[0].substr(1, lineLiteral[0].size() - 1) == literal) {
			pe.triggered = true;
			DefineDir directive{};
			directive.alias = lineLiteral[1];
			directive.symbol = lineLiteral[2];

			std::vector<DefineDir>* targetObj = static_cast<std::vector<DefineDir>*>(branch);
			targetObj->push_back(directive);

			pe.succeed = true;
		}
	}

	return pe;
}

Assemble::ParseEvent Assemble::Rule::ALIGN::ParseLiteral(std::vector<std::string>& lineLiteral)
{
	ParseEvent pe{};

	if (BeginsWithHash(lineLiteral[0])) {
		if (lineLiteral[0].substr(1, lineLiteral[0].size() - 1) == literal) {
			pe.triggered = true;
			AlignDir directive{};
			directive.sec = Util::mapSectionStr(lineLiteral[1]);
			directive.multipleOf = std::atoi(lineLiteral[2].c_str());

			if (directive.multipleOf == 0) {
				pe.severty = ParseEvent::Severty::CRITICAL;
				pe.errorStr = "The alignment offset must be greater than one.";
				return pe;
			}
			if (directive.sec == AlignDir::Section::ERRORTYPE) {
				pe.severty = ParseEvent::Severty::CRITICAL;
				pe.errorStr = "The given segement cannot be mapped.";
				return pe;
			}

			std::vector<AlignDir>* targetObj = static_cast<std::vector<AlignDir>*>(branch);
			targetObj->push_back(directive);

			pe.succeed = true;
		}
	}

	return pe;
}

Assemble::ParseEvent Assemble::Rule::ORG::ParseLiteral(std::vector<std::string>& lineLiteral)
{
	ParseEvent pe{};

	if (lineLiteral[0] == "org") {
		pe.triggered = true;
		OrganizeDir directive{};
		directive.sec = Util::mapSectionStr(lineLiteral[1]);
		Util::ValueReturnObject<long> vro = Util::convertToDecimal<long>(lineLiteral[2].c_str());
		if (!vro.isValid) {
			pe.severty = ParseEvent::Severty::CRITICAL;
			pe.errorStr = "The given postfix indicating a number system does not match the given pattern ('q' 'h', 'o', 'b', 'd')\n";
			return pe;
		}

		if (directive.sec == AlignDir::Section::ERRORTYPE) {
			pe.severty = ParseEvent::Severty::CRITICAL;
			pe.errorStr = "The given segement cannot be mapped.";
			return pe;
		}

		directive.offset = vro.value;

		std::vector<OrganizeDir>* targetObj = static_cast<std::vector<OrganizeDir>*>(branch);
		targetObj->push_back(directive);

		pe.succeed = true;
	}

	return pe;
}

Assemble::ParseEvent Assemble::Rule::Section::ParseLiteral(std::vector<std::string>& lineLiteral)
{
	ParseEvent pe{};

	if (lineLiteral[0] == "section") {
		pe.triggered = true;

		AlignDir::Section sec = Util::mapSectionStr(lineLiteral[1]);

		if (sec == AlignDir::Section::ERRORTYPE || sec == AlignDir::Section::NONE) {
			pe.errorStr = "The given section is not valid.\n";
			pe.severty = ParseEvent::Severty::CRITICAL;
			return pe;
		}

		pe.sectionInit = sec;
		pe.succeed = true;
	}

	return pe;
}

Assemble::ParseEvent Assemble::Rule::Label::ParseLiteral(std::vector<std::string>& lineLiteral)
{
	ParseEvent pe{};

	if (lineLiteral[0][0] == '_' && lineLiteral[0][lineLiteral[0].size() - 1] == ':') {
		pe.triggered = true;

		std::map<std::string, void*>* targetObj = static_cast<std::map<std::string, void*>*>(branch);
		const std::string& substr = lineLiteral[0].substr(1, lineLiteral[0].size() - 2);
		targetObj->emplace(substr, nullptr);
		pe.labelRef = substr;

		pe.succeed = true;
	}

	return pe;
}

Assemble::ParseEvent Assemble::Rule::DefineByte::ParseLiteral(std::vector<std::string>& lineLiteral)
{
	ParseEvent pe{};

	if (lineLiteral[0] == "db") {
		pe.triggered = true;

		std::vector<Bytestream>* targetObj = static_cast<std::vector<Bytestream>*>(branch);
		targetObj->push_back({});
		Bytestream& stream = (*targetObj)[targetObj->size() - 1];

		for (int i = 1; i < lineLiteral.size(); i++) {
			const std::string& word = lineLiteral[i];
			if (word[0] == '"' && word[word.size() - 1] == '"') {
				for (int j = 1; j < word.size() - 1; j++) {
					stream.bytestream.push_back(word[j]);
				}
			}
			else {
				Util::ValueReturnObject vro = Util::convertToDecimal<char>(word);
				if (!vro) {
					pe.severty = ParseEvent::Severty::WARNING;
					pe.errorStr = "Cannot parse value that has been defined by db-directive. Assuming 0.";
					return pe;
				}
				stream.bytestream.push_back(vro.value);
			}
			stream.lenght = stream.bytestream.size();
		}

		pe.labelRefObj = (void*)&stream;
		pe.ptrType = LabelPointer::DataType::DEFINE_BYTE;
		pe.succeed = true;
	}

	return pe;
}

Assemble::ParseEvent Assemble::Rule::ReserveByte::ParseLiteral(std::vector<std::string>& lineLiteral)
{
	ParseEvent pe{};

	if (lineLiteral[0] == "resb") {
		pe.triggered = true;

		std::vector<BytestreamRes>* targetObj = static_cast<std::vector<BytestreamRes>*>(branch);
		targetObj->push_back({});

		Util::ValueReturnObject vro = Util::convertToDecimal<size_t>(lineLiteral[1]);

		if (!vro) {
			pe.severty = ParseEvent::Severty::CRITICAL;
			pe.errorStr = "Cannot parse value that has been defined by resb-directive.";
			return pe;
		}

		BytestreamRes& stream = (*targetObj)[targetObj->size() - 1];
		stream.lenght = vro.value;

		pe.labelRefObj = (void*)&stream;
		pe.ptrType = LabelPointer::DataType::RESERVE_BYTE;
		pe.succeed = true;
	}

	return pe;
}

Assemble::ParseEvent Assemble::Rule::InstructionDir::ParseLiteral(std::vector<std::string>& lineLiteral)
{
	const InstructionInformation& info = getInstructionData(lineLiteral[0]);

	ParseEvent pe{};
	if (info.isValid) {
		pe.triggered = true;

		Instruction instr = {};
		instr.opcode = info.opcode;

		if (info.getWordCount() != lineLiteral.size()) {
			pe.severty = ParseEvent::Severty::CRITICAL;
			pe.errorStr = std::string_view("The given operands for instruction " + lineLiteral[0] + " are invalid! Please check the documentation.\n");
			return pe;
		}

		auto field = info.operandBitfield;
		if (field & 0b0010'0000) {
			if (field & 0b0100'0000) {
				instr.param0 = { Operand::Type::SYSTEM_CONSTANT, new Op_SystemConstant(field & 0b0001'0000) };
			}
			else {
				instr.param1 = { Operand::Type::SYSTEM_CONSTANT, new Op_SystemConstant(field & 0b0001'0000) };
			}
		}

		bool isComplex = field & 0b1000'0000;
		unsigned char operandFetch = isComplex ? 3 : 4;

		unsigned char possibles[4] = { 0b0000'1000 , 0b0000'0100 , 0b0000'0010 , 0b0000'0001 };
		unsigned char opCount = 1;
		for (int i = 0; i < operandFetch; i++) {
			if (field & possibles[i]) {
				auto paramResponse = Util::dispatchOperand(lineLiteral[opCount++]);

				switch (i) {
				case 0:
					instr.param0 = paramResponse;
					break;
				case 1:
					instr.param1 = paramResponse;
					break;
				case 2:
					instr.op0 = paramResponse;
					break;
				case 3:
					instr.op1 = paramResponse;
					break;
				}
			}
		}

		std::vector<Instruction>* targetObj = static_cast<std::vector<Instruction>*>(branch);
		targetObj->push_back(instr);

		pe.labelRefObj = &targetObj[targetObj->size() - 1];
		pe.ptrType = LabelPointer::DataType::INSTRUCTION;
		pe.succeed = true;
	}

	return pe;
}