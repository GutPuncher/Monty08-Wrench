# Monty08 Wrench

Is the official assembler for the 8-Bit microprocessor [Monty08](https://github.com/timmy0811/Monty08) that converts M08-Assembly into executable binaries for the Monty08. This project now consists of a parser for the M08-Assembly language and an assembler for generating the needed binaries that can be then uploaded onto the main memory of the microprocessor. Looking into the future I plan to also implement a parser for a dedicated high level language like C/C++ with and additional optimizer.

## Features
- Custom M08-Assembly language inspired by the intel x86 asm
- Dynamic Data, BSS and Text sections that can be arranged in their order and alignment
- Virtual instructions emulated on the CPU that expand the ISA of the Monty08
- Adjustable output and code size
- verbose syntax checkup of source file

## How to use
### Building
This project uses Premake to ensure a platform independent usage. Execute the *Setup*-Script inside the *Scripts*-Folder to generate the project for your platform.

### Usage
Either you are building this project yourself or taking the release - binaries,  you just open up a terminal and use the additional command line arguments to control the behaviour of this application.
Use ```wrench --help``` to list the valid arguments:

```
Usage: wrench source_file.asm [arg_name...]

Possible arguments:

        -o, --output:           An absolut or relative path to the file that gets outputted.
        -s, --size:             Size of the compiled binary program.
        -c, --code:             Size of the text section (code).
```

The assembler then parses the given source file and assembles it into a ```.m08``` file, as defined using the ```-o``` argument.

## The M08-Assembly language
As already mentioned the Assembly language is highly inspired by existing languages and covers the most important elements of them. The following example shows all the features contained in the M08-Language:

```
#align .text 8
#define register0 r0

org .text 8

section .data
_string:
	db "this_is_a_string"
	
section .bss
_data:
	resb 10h
			
section .text
	ldav 5
	zero
	xchop
	
_loop:
	inc
	jmpne [loop]
	
	end
	nop
```
Instructions starting with a ```#``` indicate a preprocessor directive that gets parsed before the main parsing cycle and contain meta-information about the program itself.

---
### General conventions and formats
- All numeric values including the command line arguments can be represented in different number-systems using different postfixes: 'o' = octal, 'h' = hex, 'b' = binary, 'q' = quad, 'd' or blank = decimal.
- Sections are case-insensitive, the dot is optional, means: ```.text = .Text = TEXT = text```.
- Available registers (case-insensitive): ```r0``` and ```r1```
- Available ports (case-insensitive): ```p0``` and ```p1```
- labels referenced in instruction operand are enclosed by ```[]```, means jumping to a label looks like this: ```jmp [sample_label]```

---
### Alignement
**Syntax:** ```#align [section] [numeric_multiple]```

**Description:** Aligns the given section to start at a multiple of ```numeric_multiple``` bytes.

---
### Defines
**Syntax:** ```#define [symbol] [alias]```

**Description:** Creates an alias that gets then replaced by ```symbol``` during the assembly stage.

---
### Organization
**Syntax:** ```org [section] [numeric_offset]```

**Description:** Defines the offset at which a specific section begins. Offset = 0 means the section starts at address 0 of the program. Be aware of possible conflicts between two or more sections that collide.

---
### Labels
**Syntax:** ```_[label]:```

**Description:** Creates a symbolic label representing the address of the next Instruction/Definebyte/Reservebyte in the context of the assembled program.

---
### Sections
**Syntax:** ```section [section]```

**Description:** Introduces one of the three sections: [.text/.bss/.data]. The program stays in the same section until a new section is introduced.
*BSS*: uninitialized data (only ```resb``` is valid in this section), *DATA*: data initializied with a non zero value (only ```db``` is valid in this section), *TEXT*: code of the program

---
### Define-Bytes
**Syntax:** ```db [bytestream]```

**Example:** ```db 16h 44o 12```, ```db "string"```

**Description:** Places the given ```bytestream``` directly into the program as raw data that can then be later accessed by the microprocessor.

---
### Reserve-Bytes
**Syntax:** ```resb [numeric_size]```

**Description:** Allocates space of size ```numeric_size``` inside the programs memory space that can then be written to. A label should point to that memory region.

---
### Instructions
**Syntax:** ```[instruction] [param0] [param1] [operand0] [operand1]```

**Description:** A single instruction as part of the code consisting of a ```instruction``` defined by the [Monty08-ISA](https://github.com/timmy0811/Monty08) and additional arguments that are optional. Read the ISA for additional information on all available instructions and their parameters.
