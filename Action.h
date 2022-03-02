#pragma once
#include "pch.h"

// Make a macro for printing out things it is used for Debugging Purposes 
#define PRINT(x) std::cout<<x<<std::endl;

namespace HelperFunc {

	// returns true or false on whether the current input line is a function header
	bool isFuncHeaderInLine(std::string& line);
	
	// Breaks the string based on the breaker char and stores the pieces into storage 
	// It acts like the python Delimeter function VERY IMPORTANT FUNCTION !!!!!!!!!!!!!!!!
	void breakString(std::string& line, const char breaker, std::vector<std::string>& storage);

	// displays the local variables and their offsets stored which are stored in the vector of pairs 
	// It is used for debugging purposes 
	void display(std::vector<std::pair<std::string, short> > & vars);

	// finds the offset of a local variable based on looking into a vector of pairs
	short getOffset(std::vector<std::pair<std::string, short> >& vars, std::string& variableName);

	// finds the value of a local variable based on looking into a vector of pairs
	int getValue(std::vector<std::pair<std::string, int> >& vars, std::string &variableName);

	// Function that handles the return statement it updates the output and looks for the offset of the returned value 
	void handleReturnStatement(std::vector<std::string>& answer, std::string &returnStatement, std::vector<std::pair<std::string, short>> & localVarsOffsets);
}
