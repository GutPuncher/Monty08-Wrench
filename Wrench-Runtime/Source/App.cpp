#include "Assembler/Component.h"
#include "Compiler/CompileComp.h"

int main(int argc, char** argv) {
	Assemble::Assemble("sampleData/sample3.txt", "binOut/out.hex");

	return 0;
}