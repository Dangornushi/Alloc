#include "Alloc.h"

bool Parser::isNumber(string str) {
	for (const char& tmp : str)
		if (!std::isdigit(tmp))
			return false;

	return true;
}
