#include "Assembler/AssembleComp.h"
#include "Preprocessor/PreprocessComp.h"
#include "Parser/ParseComp.h"

int main()
{
	Assemble::PrintHelloWorld();
	Parse::PrintHelloWorld();
	Preprocess::PrintHelloWorld();
}