#pragma once

#include "Parser/Parser.h"

#include <iostream>

namespace Assemble {
	void Assemble() {
		Parser ps{};
		ps.LoadFromFile("sampleData/sample3.txt");
		if (!ps.Parse(true)) std::cout << "=> The parsing process had been aborted. There might be more errors.\n";
		if (!ps.Parse(false)) std::cout << "=> The parsing process had been aborted. There might be more errors.\n";
	}
}