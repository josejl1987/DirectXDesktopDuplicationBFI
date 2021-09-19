#pragma once
#include "expected.h"

class ErrorInfo {
public:
	std::string ErrorMessage;

public:
	ErrorInfo(std::string&& message) { this->ErrorMessage = message; }
};

template <class T>
using Result = tl::expected<T, ErrorInfo>;