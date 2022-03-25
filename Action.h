#pragma once
#include "pch.h"

// Make a macro for printing out things it is used for Debugging Purposes 
#define PRINT(x) std::cout<<x<<std::endl;

// namespace below has all the helper functions that I need 
namespace HelperFunc {
	// returns true or false on whether the current input line is a function header
	bool isFuncHeaderInLine(std::string& line);
	
	// Breaks the string based on the breaker char and stores the pieces into storage 
	// It acts like the python Delimeter function VERY IMPORTANT FUNCTION !!!!!!!!!!!!!!!!
	void breakString(std::string& line, const char breaker, std::vector<std::string>& storage);

	// displays the local variables and their offsets stored which are stored in the vector of pairs 
	// It is used for debugging purposes 
	void display(std::vector<std::pair<std::string, short> > & vars);

	// displays the local variables and their values stored which are stored in the vector of pairs 
	void displayVarNValues(std::vector<std::pair<std::string, int> >& vars);

	// finds the offset of a local variable based on looking into a vector of pairs
	short getOffset(std::vector<std::pair<std::string, short> >& vars, std::string& variableName);

	// finds the value of a local variable based on looking into a vector of pairs
	int getValue(std::vector<std::pair<std::string, int> >& vars, std::string &variableName);

	// Sets the value of a local variable or an array element
	void setValueOfVar(std::vector<std::pair<std::string, int>>& VarValues, std::string & varName, int newValue);

	// Convert Arrayname and Index to special format name index in the form array[index]
	std::string convertArrName(std::string & originalName);

	// Returns what operator is in the line
	std::string whatOperator(std::string& operationLine);

	// Handle simple arithmetic operations +,-,*,/
	void HandleSimpleArithmetic(std::vector<std::string>& answervector, std::string& operationstatement, std::vector<std::pair<std::string, short>>& localVarsOffsets, std::vector<std::pair<std::string, int>>& VarValues);

	// Handle 1st part of For Loop adds loop variable counter in assembly code  
	void handleFirstPart(std::string& part, std::vector<std::string>& answervector, std::vector<std::pair<std::string, short>>& localVarsOffsets, short&off, std::vector<std::pair<std::string, int>> &VarValues);

	// Handle 2nd part of For Loop and add first two lines of assembly code under the begining loop label  
	void handleSecondPart(std::string& part, std::vector<std::string>& answervector, short &labelNum, short& off);

	// Handle 3nd part of For Loop and adds the increment value that changes the value of the for loop variable 
	void handleThirdPart(std::string& part, std::vector<std::string>& answervector,short& off);


	// Function that handles the return statement it updates the output and looks for the offset of the returned value 
	void handleReturnStatement(std::vector<std::string>& answer, std::string &returnStatement, std::vector<std::pair<std::string, short>> & localVarsOffsets);
}

