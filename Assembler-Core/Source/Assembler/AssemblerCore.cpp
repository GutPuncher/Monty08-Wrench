#include "AssemblerCore.h"

#include <fmt/format.h>
#include <fmt/color.h>

#include <iterator>
#include <algorithm>

bool Assemble::Assembler::AssembleFromTree(const ParseTree* tree, const std::string& outFile)
{
	fmt::print("Assembling objects.\n----------------------------------------------\n");

	auto organizers = tree->organizes;

	SectionInfo text{
		AlignDir::Section::TEXT,
		m_MaxProgSize,
		0,
		true,
		generateText
	};

	SectionInfo bss{
		AlignDir::Section::BSS,
		GetBssSize(tree),
		m_MaxProgAdr - GetBssSize(tree),
		tree->sec_bss.dataRes.size() != 0,
		generateBss
	};

	SectionInfo data{
		AlignDir::Section::DATA,
		GetDataSize(tree),
		m_MaxProgAdr - GetDataSize(tree) - bss.size,
		tree->sec_data.data.size() != 0,
		generateData
	};

	for (auto organizer : organizers) {
		switch (organizer.sec) {
		case Assemble::AlignDir::Section::BSS:
			bss.offset = organizer.offset;
			break;
		case Assemble::AlignDir::Section::DATA:
			data.offset = organizer.offset;
			break;
		case Assemble::AlignDir::Section::TEXT:
			text.offset = organizer.offset;
			break;
		}
	}

	std::vector<SectionInfo> sects{ text, bss, data };
	std::sort(sects.begin(), sects.end());

	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			if (i == j) continue;

			if (sects[i].doesExist && sects[j].doesExist) {
				if (int overlapLength = CalcIntersectionLenght(sects[i].offset, sects[j].offset, sects[i].offset + sects[i].size, sects[j].offset + sects[j].size);
					overlapLength != -1) {
					fmt::print(fg(fmt::color::crimson), "[Error]: Section {0} colliding with section {1}. Sections overlaping by {2} bytes. Fragmentation is not supported as now.\n", MapIndexToSection(i), MapIndexToSection(j), overlapLength);
					CloseHandle(m_ExecutableHandle);
					return false;
				}
			}
		}
	}

	m_ExecutableHandle = std::ofstream(outFile, std::ios::binary);
	if (!m_ExecutableHandle.is_open()) {
		fmt::print(fg(fmt::color::crimson), "Cannot open file {}\n", outFile);
		CloseHandle(m_ExecutableHandle);
		return false;
	}

	int lastEnd = 0;
	for (int i = 0; i < 3; i++) {
		int offset = sects[i].offset;
		int interval = offset - lastEnd;
		std::fill_n(std::ostreambuf_iterator<char>(m_ExecutableHandle), interval, static_cast<char>(0x3F));

		m_BufferPtr += (size_t)interval;

		if (!sects[i].generate(tree, m_ExecutableHandle, m_BufferPtr, m_LabelIndex, m_LabelReg)) {
			CloseHandle(m_ExecutableHandle);
			return false;
		}

		lastEnd = offset + sects[i].size;
	}

	if (!MatchLabels()) {
		return false;
	}

	CloseHandle(m_ExecutableHandle);

	return true;
}

inline void Assemble::Assembler::CloseHandle(std::ofstream& handle)
{
	if (handle) {
		handle.close();
	}
}

size_t Assemble::Assembler::GetBssSize(const ParseTree* tree)
{
	auto data = tree->sec_bss;
	size_t size = 0;

	for (Assemble::BytestreamRes stream : data.dataRes) {
		size += stream.lenght;
	}

	return size;
}

size_t Assemble::Assembler::GetDataSize(const ParseTree* tree)
{
	auto data = tree->sec_data;
	size_t size = 0;

	for (Assemble::Bytestream stream : data.data) {
		size += stream.lenght;
	}

	return size;
}

std::string Assemble::Assembler::MapIndexToSection(int index) const
{
	switch (index) {
	case 0:
		return "text";
	case 1:
		return "bss";
	case 2:
		return "data";
	default:
		return "null";
	}
}

int Assemble::Assembler::CalcIntersectionLenght(int start0, int start1, int end0, int end1) const
{
	if (end0 > start1 && start0 < end1) {
		return std::min(end0 - start1, end1 - start0);
	}

	return -1;
}

bool Assemble::Assembler::generateText(const ParseTree* tree, std::ofstream& stream, size_t& bufPtr, index& ind, registry& reg)
{
	for (Instruction instr : tree->sec_text.code) {
		char opbyte = instr.opcode;

		if (instr.label.size()) {
			ind.insert({ instr.label, bufPtr });
		}

		if (instr.param0.second) {
			char val = matchParamValToBin(instr.param0.second, instr.param0.first, 6);
			if (val == -1) return false;
			opbyte |= val;
		}

		if (instr.param1.second) {
			char val = matchParamValToBin(instr.param1.second, instr.param1.first, 7);
			if (val == -1) return false;
			opbyte |= val;
		}

		bufPtr++;
		stream << opbyte;

		if (instr.op0.second) {
			BinaryOperand binOp = matchOpValToBin(instr.op0.second, instr.op0.first, reg, bufPtr);
			if (!binOp) return false;
			stream << binOp.binaries.first;
			bufPtr++;
			if (binOp.wide) {
				stream << binOp.binaries.second;
				bufPtr++;
			}
		}

		if (instr.op1.second) {
			BinaryOperand binOp = matchOpValToBin(instr.op1.second, instr.op1.first, reg, bufPtr);
			if (!binOp) return false;
			stream << binOp.binaries.first;
			bufPtr++;
			if (binOp.wide) {
				stream << binOp.binaries.second;
				bufPtr++;
			}
		}
	}

	return true;
}

bool Assemble::Assembler::generateBss(const ParseTree* tree, std::ofstream& stream, size_t& bufPtr, index& ind, registry& reg)
{
	for (Assemble::BytestreamRes st : tree->sec_bss.dataRes) {
		std::fill_n(std::ostreambuf_iterator<char>(stream), (int)st.lenght, '0');
		if (st.label.size()) {
			ind.insert({ st.label, bufPtr });
		}

		bufPtr += st.lenght;
	}

	return true;
}

bool Assemble::Assembler::generateData(const ParseTree* tree, std::ofstream& stream, size_t& bufPtr, index& ind, registry& reg)
{
	for (Bytestream bs : tree->sec_data.data) {
		stream.write(bs.bytestream.data(), bs.lenght);
		if (bs.label.size()) {
			ind.insert({ bs.label, bufPtr });
		}

		bufPtr += bs.lenght;
	}

	return true;
}

char Assemble::Assembler::matchParamValToBin(Operand* op, Operand::Type type, char index)
{
	char out = 0;

	switch (type) {
	case Operand::Type::PORT:
		out = (char)static_cast<Op_Port*>(op)->p;
		break;
	case Operand::Type::REGISTER:
		out = (char)static_cast<Op_Register*>(op)->reg;
		break;
	case Operand::Type::SYSTEM_CONSTANT:
		out = static_cast<Op_SystemConstant*>(op)->value;
		break;
	case Operand::Type::NUMERIC:
		out = (char)static_cast<Op_Numeric*>(op)->value;
		break;
	default:
		return -1;
	}

	return out << index;
}

Assemble::Assembler::BinaryOperand Assemble::Assembler::matchOpValToBin(Operand* op, Operand::Type type, registry& registry, size_t bufPtr)
{
	BinaryOperand bin{ true };

	switch (type) {
	case Operand::Type::LABEL: {
		Op_Label lab = *static_cast<Op_Label*>(op);
		registry.push_back({ bufPtr, lab.label });
		bin.wide = true;
		break;
	}
	case Operand::Type::NUMERIC: {
		Op_Numeric num = *static_cast<Op_Numeric*>(op);
		bin.binaries.first = num.value;
		break;
	}
	case Operand::Type::NUMERIC_WIDE: {
		Op_NumericWide numW = *static_cast<Op_NumericWide*>(op);
		bin.binaries.first = (char)(numW.value >> 8);
		bin.binaries.second = (char)((numW.value & 0x00FF) >> 8);
		bin.wide = true;
		break;
	}
	default:
		bin.isValid = false;
		return bin;
	}

	return bin;
}

bool Assemble::Assembler::MatchLabels()
{
	std::streampos adr;
	for (auto gap : m_LabelReg) {
		adr = gap.first;
		m_ExecutableHandle.seekp(adr);

		auto iter = m_LabelIndex.find(gap.second);
		if (iter != m_LabelIndex.end()) {
			size_t pos = m_LabelIndex.at(gap.second); // TODO: Optimize, use iterator to access adr
			const char lower = (char)(pos - 1 & 0x00FF);
			m_ExecutableHandle.write(&lower, 1);

			adr += 1;
			const char higher = (char)(pos >> 8);
			m_ExecutableHandle.write(&higher, 1);

			if (lower + 1 == 0 && higher == 0) {
				fmt::print(fg(fmt::color::crimson), "[Error]: A jump to address 0 (label \"{}\") is not supported. To fix this error insert a 'nop' at the start of your program or move the affected section.\n", gap.second);
				return false;
			}
		}
		else {
			fmt::print(fg(fmt::color::crimson), "[Error]: The given symbol {0} could not resolved.\n", gap.second);
			return false;
		}
	}
	return true;
}