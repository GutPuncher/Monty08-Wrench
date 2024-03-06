#include "InstructionSet.h"

Assemble::InstructionInformation Assemble::getInstructionData(std::string& instrStr)
{
	InstructionInformation* instruction = nullptr;

	auto iter = instructions.find(instrStr);
	if (iter != instructions.end()) {
		instruction = &iter->second;
	}
	else {
		InstructionInformation inf{};
		inf.isValid = false;
		return inf;
	}

	return *instruction;
}

void Assemble::SetupInstructionData()
{
	// Arithmetic and Logic
	instructions["add"] = { 0x00, 0b0000'0000, 3 };
	instructions["addc"] = { 0x01, 0b0000'0000, 3 };
	instructions["sub"] = { 0x02, 0b0000'0000, 3 };
	instructions["subb"] = { 0x03, 0b0000'0000, 3 };
	instructions["inc"] = { 0x04, 0b0000'0000, 3 };
	instructions["dec"] = { 0x05, 0b0000'0000, 3 };
	instructions["and"] = { 0x06, 0b0000'0000, 3 };
	instructions["or"] = { 0x07, 0b0000'0000, 3 };
	instructions["xor"] = { 0x08, 0b0000'0000, 3 };
	instructions["zero"] = { 0x09, 0b0000'0000, 3 };
	instructions["cpl"] = { 0x0A, 0b0000'0000, 3 };
	instructions["one"] = { 0x0B, 0b0000'0000, 3 };
	instructions["rl"] = { 0x0C, 0b0000'0000, 3 };
	instructions["rlc"] = { 0x0D, 0b0000'0000, 3 };
	instructions["cmp"] = { 0x0E, 0b0000'0000, 3 };

	// Transport
	instructions["xchop"] = { 0x10, 0b0000'0000, 2 };
	instructions["swp"] = { 0x39, 0b0000'0000, 2 };
	instructions["moda"] = { 0x11, 0b0000'1010, 4 };
	instructions["modr"] = { 0x14, 0b0010'0010, 6 };
	instructions["xchar"] = { 0x15, 0b0000'1000, 6 };
	instructions["xchrr"] = { 0x16, 0b0000'0000, 6 };
	instructions["movar"] = { 0x17, 0b0000'1000, 2 };
	instructions["movra"] = { 0x18, 0b0000'1000, 2 };
	instructions["ldad"] = { 0x19, 0b1000'0011, 4 };
	instructions["ldrd"] = { 0x1A, 0b1000'1011, 4 };
	instructions["ldasti"] = { 0x1B, 0b0000'1000, 4 };
	instructions["ldrsti"] = { 0x1C, 0b0000'1100, 4 };
	instructions["ldahpi"] = { 0x1D, 0b0000'1000, 4 };
	instructions["ldrhpi"] = { 0x1E, 0b0000'1100, 4 };
	instructions["ldav"] = { 0x1F, 0b0000'0010, 2 };
	instructions["ldrv"] = { 0x20, 0b0000'1010, 2 };
	instructions["stoad"] = { 0x21, 0b1000'0011, 4 };
	instructions["stord"] = { 0x22, 0b1000'1011, 4 };
	instructions["stoasti"] = { 0x23, 0b0000'1000, 4 };
	instructions["storsti"] = { 0x24, 0b0000'1100, 4 };
	instructions["stoahpi"] = { 0x25, 0b0000'1000, 4 };
	instructions["storhpi"] = { 0x26, 0b0000'1100, 4 };

	// Branching and Conditional
	instructions["jmp"] = { 0x27, 0b1000'0011, 3 };
	instructions["jmpc"] = { 0x28, 0b1111'0011, 3 };
	instructions["jmpz"] = { 0x29, 0b1111'0011, 3 };
	instructions["jmpe"] = { 0x2A, 0b1111'0011, 3 };

	// --> Virtual Instructions
	instructions["jmpnc"] = { 0x28, 0b1111'0011, 3, 0b0000'0001 };
	instructions["jmpnz"] = { 0x29, 0b1111'0011, 3, 0b0000'0001 };
	instructions["jmpne"] = { 0x2A, 0b1111'0011, 3, 0b0000'0001 };

	// I/O
	instructions["portwrt"] = { 0x2C, 0b0000'1000, 2 };
	instructions["portrd"] = { 0x2D, 0b0000'1000, 2 };
	instructions["pshvb"] = { 0x30, 0b0010'0000, 2 };
	instructions["ldvpv"] = { 0x31, 0b1010'0011, 3 };
	instructions["movvpa"] = { 0x32, 0b0000'1000, 2 };
	instructions["movavp"] = { 0x33, 0b0000'1000, 2 };

	// Subroutines
	instructions["call"] = { 0x3A, 0b1000'0011, 6 };
	instructions["ret"] = { 0x3B, 0b0000'0000, 6 };

	// Stack
	instructions["push"] = { 0x3C, 0b0000'0000, 2 };
	instructions["pop"] = { 0x3D, 0b0000'0000, 3 };

	// Operation
	instructions["nop"] = { 0x3E, 0b0000'0000, 2 };
	instructions["end"] = { 0x3F, 0b0000'0000, 0 };
}