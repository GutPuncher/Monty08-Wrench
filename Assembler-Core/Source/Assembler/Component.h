#pragma once

#include "Parser/Parser.h"
#include "AssemblerCore.h"

#include <iostream>
#include <string>

#include <fmt/format.h>
#include <fmt/color.h>

namespace Assemble {
	void Assemble(const std::string& path, const std::string& binaries, size_t progSize, size_t codeSize, bool logisimOut) {
		Parser ps{};
		ps.LoadFromFile(path);
		if (!ps.Parse(true)) std::cout << "=> The parsing process has been aborted. There might be more errors.\n";
		else {
			if (!ps.Parse(false)) std::cout << "=> The parsing process has been aborted. There might be more errors.\n";
			else {
				Assembler as{};
				as.SetData(progSize, codeSize);
				as.ToggleLogisimOut(logisimOut);
				if (!as.AssembleFromTree(ps.getTree(), binaries)) std::cout << "=> The assembly process has been aborted.\n";
				else {
					fmt::print(fg(fmt::color::green), "=> Successfully assembled binaries for {}\n", path);
				}

				if (logisimOut) {
					if (as.GenerateLogisim())
						fmt::print(fg(fmt::color::green), "=> Successfully generated Logisim output for {}\n", path);
					else
						std::cout << "=> Could not generate Logisim output.";
				}
			}
		}
	}
}