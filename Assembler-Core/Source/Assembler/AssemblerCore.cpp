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
					return false;
				}
			}
		}
	}

	m_ExecutableHandle = std::ofstream(outFile, std::ios::binary);
	if (!m_ExecutableHandle.is_open()) {
		fmt::print(fg(fmt::color::crimson), "Cannot open file {}\n", outFile);
		return false;
	}

	int lastEnd = 0;
	for (int i = 0; i < 3; i++) {
		int offset = sects[i].offset;
		std::fill_n(std::ostreambuf_iterator<char>(m_ExecutableHandle), offset - lastEnd, static_cast<char>(0x3F));
		sects[i].generate(tree, m_ExecutableHandle);
		lastEnd = offset + sects[i].size;
	}

	m_ExecutableHandle.close();

	return true;
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

bool Assemble::Assembler::generateText(const ParseTree* tree, std::ofstream& stream)
{
	std::fill_n(std::ostreambuf_iterator<char>(stream), 0x20, 'T');
	return false;
}

bool Assemble::Assembler::generateBss(const ParseTree* tree, std::ofstream& stream)
{
	std::fill_n(std::ostreambuf_iterator<char>(stream), GetBssSize(tree), '0');
	return false;
}

bool Assemble::Assembler::generateData(const ParseTree* tree, std::ofstream& stream)
{
	for (Bytestream bs : tree->sec_data.data) {
		stream.write(bs.bytestream.data(), bs.lenght);
	}
	return false;
}