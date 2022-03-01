#pragma once
#include "pch.h"

namespace HelperFunc {

	// returns true or false on whether the current input line is a fucntion header
	bool isFuncHeaderInLine(std::string& line);
	
	// Breaks the string based on the breaker char and stores the pieces into storage 
	// It acts like the python Delimeter function VERY IMPORTANT FUNCTION !!!!!!!!!!!!!!!!
	void breakString(std::string& line, const char breaker, std::vector<std::string>& storage);
    
}
