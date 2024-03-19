#include "Assembler/Component.h"
#include "Compiler/CompileComp.h"
#include "Assembler/Parser/Util.h"

#include <fmt/format.h>
#include <fmt/color.h>

#include <filesystem>
#include <fstream>

void printHelp();
inline void printArg(const std::string& arg, const std::string& argVerb, const std::string& desc);

#define VER_STR "MontyWrench-1.0"
#define EXE_STR "wrench"
#define AUTH_STR "FEZ"

int main(int argc, char** argv) {
	if (argc < 2) {
		fmt::print("You must provide a file to be assembled.\nUse -h or --help for details.");
		return 0;
	}

	std::string sourcePath = argv[1];
	std::string outPath = std::filesystem::path(argv[1]).filename().stem().string() + ".m08";

	size_t progSize = 0x8000;
	size_t codeSize = 0x4000;
	bool ls = false;

	bool microcode = true;

	for (int i = 1; i < argc; i++) {
		std::string arg = argv[i];

		if (arg == "-h" || arg == "--help" || arg == "help") {
			printHelp();
			return 0;
		}
		else if (arg == "-o" || arg == "--output") {
			outPath = argv[i + 1];
		}
		else if (arg == "-s" || arg == "--size") {
			Assemble::Util::ValueReturnObject vro = Assemble::Util::convertToDecimal<long>(argv[i + 1]);
			if (!vro) {
				fmt::print(fg(fmt::color::crimson), "Invalid argument for {0}: value '{1}' cannot be converted.", arg, argv[++i]);
				return 0;
			}
			progSize = (size_t)vro.value;
		}
		else if (arg == "-c" || arg == "--c") {
			Assemble::Util::ValueReturnObject vro = Assemble::Util::convertToDecimal<long>(argv[i + 1]);
			if (!vro) {
				fmt::print(fg(fmt::color::crimson), "Invalid argument for {0}: value '{1}' cannot be converted.", arg, argv[++i]);
				return 0;
			}
			codeSize = (size_t)vro.value;
		}
		else if (arg == "-m" || arg == "-nomc") {
			microcode = false;
		}
		else if (arg == "-l" || arg == "--logisim") {
			ls = true;
		}
		else {
			std::ifstream check(sourcePath);
			if (!check) {
				fmt::print(fg(fmt::color::crimson), "Invalid argument for {0}. If this argument is the source file, it cannot be opened.", arg);
				return 0;
			}
		}
	}

	Assemble::Assemble(sourcePath, outPath, progSize, codeSize, ls, microcode);

	return 0;
}

void printHelp() {
	fmt::print("Welcome to {0}!\n", VER_STR);
	fmt::print("This is the official assembler for the Monty08 microprocessor developed by {0}. It parses Monty08 - Assembly files and converts them into binary programs that can be uploaded onto the microprocessor.\n\n", AUTH_STR);

	fmt::print("Usage: {0} source_file.asm [arg_name...]\n\n", EXE_STR);

	fmt::print("Possible arguments:\n\n");

	printArg("o", "output", "An absolut or relative path to the file that gets outputted.");
	printArg("s", "size", "Size of the compiled binary program.");
	printArg("c", "code", "Size of the text section (code).");
	printArg("l", "logisim", "Generates an additional hex file that can directly be loaded by Logisim.");
	printArg("m", "nomc", "Uses the arithmetic instructions for the non-microcode ALU.");
}

void printArg(const std::string& arg, const std::string& argVerb, const std::string& desc) {
	fmt::print("\t-{0}, --{1}:\t\t{2}\n", arg, argVerb, desc);
}