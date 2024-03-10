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

#ifdef sym // Not suitable for number systems containing letters, obsolete
		for (int i = 0; i < numberStr.size() - 1; i++) {
			if (std::isdigit(numberStr[i]) == 0) {
				vro.isValid = false;
				return vro;
			}
		}
#endif

		try {
			if (std::isdigit(postfix) > 0 && numberStr.find_first_not_of("0123456789") == std::string::npos) {
				vro.value = static_cast<T>(std::atoll(numberStr.c_str()));
			}
			else {
				switch (postfix) {
				case 'd':
					if (numberStr.find_first_not_of("0123456789") >= numberStr.size() - 1) {
						vro.value = static_cast<T>(std::atoll(numberStr.c_str()));
						break;
					}
				case 'b':
					if (numberStr.find_first_not_of("01") >= numberStr.size() - 1) {
						vro.value = static_cast<T>(std::stoll(numberStr.c_str(), nullptr, 2));
						break;
					}
				case 'h':
					if (numberStr.find_first_not_of("0123456789abcdef") >= numberStr.size() - 1) {
						vro.value = static_cast<T>(std::stoll(numberStr.c_str(), nullptr, 16));
						break;
					}
				case 'o':
					if (numberStr.find_first_not_of("01234567") >= numberStr.size() - 1) {
						vro.value = static_cast<T>(std::stoll(numberStr.c_str(), nullptr, 8));
						break;
					}
				case 'q':
					if (numberStr.find_first_not_of("0123") >= numberStr.size() - 1) {
						vro.value = static_cast<T>(std::stoll(numberStr.c_str(), nullptr, 4));
						break;
					}
				default:
					vro.isValid = false;
				}
			}
		}
		catch (std::invalid_argument) {
			vro.isValid = false;
		}

		return vro;
	}

	std::pair<Operand::Type, Operand*> dispatchOperand(std::string& word);
}