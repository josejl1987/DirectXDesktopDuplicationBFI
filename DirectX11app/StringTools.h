#pragma once
#include <algorithm>
#include <cctype>
#include <iomanip>
#include <sstream>
#include <string>

bool to_bool( std::string str) {
	std::transform(str.begin(), str.end(), str.begin(), ::tolower);
	std::istringstream is(str);
	bool b;
	is >> std::boolalpha >> b;
	return b;
}

uint32_t to_uint32(std::string str) {
	std::transform(str.begin(), str.end(), str.begin(), ::tolower);
	std::remove(str.begin(), str.end(), '"');
	std::remove(str.begin(), str.end(), '\'');

	return std::stoi(str);
}