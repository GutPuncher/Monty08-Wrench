#include "AssemblerCore.h"

#include <fmt/format.h>
#include <fmt/color.h>

bool Assemble::Assembler::AssembleFromTree(const ParseTree* tree)
{
	fmt::print("Assembling objects.\n----------------------------------------------\n");

	auto organizers = tree->organizes;

	SectionInfo text{
		AlignDir::Section::TEXT,
		m_MaxProgSize,
		0,
		true
	};

	SectionInfo bss{
		AlignDir::Section::BSS,
		GetBssSize(tree),
		m_MaxProgAdr - GetBssSize(tree),
		tree->sec_bss.dataRes.size() != 0
	};

	SectionInfo data{
		AlignDir::Section::DATA,
		GetDataSize(tree),
		m_MaxProgAdr - GetDataSize(tree) - bss.size,
		tree->sec_data.data.size() != 0
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

	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			if (i == j) continue;

			if (sects[i].doesExist && sects[j].doesExist) {
				if (int overlapLength = CalcIntersectionLenght(sects[i].offset, sects[j].offset, sects[i].offset + sects[i].size, sects[j].offset + sects[j].size);
					overlapLength != -1) {
					fmt::print(fg(fmt::color::crimson), "[Error]: Section {0} colliding with section {1}. Sections overlaping by {2} bytes. Fragmentation is not supported as now.\n", MapIndexToSection(i), MapIndexToSection(j), overlapLength);
					return false;
				}
			}
		}
	}

	return true;
}

size_t Assemble::Assembler::GetBssSize(const ParseTree* tree) const
{
	auto data = tree->sec_bss;
	size_t size = 0;

	for (Assemble::BytestreamRes stream : data.dataRes) {
		size += stream.lenght;
	}

	return size;
}

size_t Assemble::Assembler::GetDataSize(const ParseTree* tree) const
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