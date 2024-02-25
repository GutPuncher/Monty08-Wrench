#pragma once

#include "ParseTree.h"

#include <vector>
#include <string>
#include <iostream>
#include <utility>

namespace Assemble::Util {
	size_t split(const std::string& str, std::vector<std::string>& outlist, char sep);
	AlignDir::Section mapSectionStr(const std::string& str);
	std::string mapStrSection(AlignDir::Section section);
	std::string trim(const std::string& str, const std::string& whitespace = " \t");
	std::string reduce(const std::string& str, const std::string& fill = " ", const std::string& whitespace = " \t");

	template <typename T>
	struct ValueReturnObject {
		bool isValid;
		T value;

		explicit operator bool() const {
			return isValid;
		}
	};

	template<typename T>
	ValueReturnObject<T> convertToDecimal(const std::string& numberStr)
	{
		char postfix = std::tolower(numberStr[numberStr.size() - 1]);
		ValueReturnObject<T> vro{ true, 0 };

		for (int i = 0; i < numberStr.size() - 1; i++) {
			if (std::isdigit(numberStr[i]) == 0) {
				vro.isValid = false;
				return vro;
			}
		}

		if (std::isdigit(postfix) > 0) {
			vro.value = static_cast<T>(std::atoll(numberStr.c_str()));
		}
		else if (postfix == 'd') {
			vro.value = static_cast<T>(std::atoll(numberStr.substr(0, numberStr.size() - 1).c_str()));
		}
		else {
			switch (postfix) {
			case 'b':
				vro.value = static_cast<T>(std::stoll(numberStr.c_str(), nullptr, 2));
				break;
			case 'h':
				vro.value = static_cast<T>(std::stoll(numberStr.c_str(), nullptr, 16));
				break;
			case 'o':
				vro.value = static_cast<T>(std::stoll(numberStr.c_str(), nullptr, 8));
				break;
			case 'q':
				vro.value = static_cast<T>(std::stoll(numberStr.c_str(), nullptr, 4));
				break;
			default:
				vro.isValid = false;
			}
		}

		return vro;
	}

	std::pair<Operand::Type, Operand*> dispatchOperand(std::string& word);
}