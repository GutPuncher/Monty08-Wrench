#pragma once

#include "Parser/Parser.h"

namespace Assemble {
	void Assemble() {
		Parser ps{};
		ps.LoadFromFile("sampleData/sample3.txt");
		ps.Parse(true);
	}
}