#include "pch.h"

#include "Action.h"

namespace HelperFunc { 

	// returns true or false on whether the current input line is a function header
	bool HelperFunc::isFuncHeaderInLine(std::string& line) {
		return ((line.find("(") != -1) && (line.find(")") != -1) && (line.find("while")==-1) && (line.find("for") == -1) && (line.find(";") == -1)) ? true : false;
	}

	// Breaks the string based on the breaker char and stores the pieces into storage 
	void breakString(std::string& line, char breaker, std::vector<std::string>& storage) {
		std::stringstream list(line);
		std::string out;
		while (std::getline(list, out, breaker)) {
			storage.push_back(out);
		}
	}

	// displays the local variables and their offsets stored which are stored in the vector of pairs 
	void display(std::vector<std::pair<std::string, short>>& vars) {
		for (std::pair<std::string, short> &element : vars) {
			std::cout << element.first << "   ";
			std::cout << element.second <<std::endl;
		}
	}

	// displays the local variables and their values stored which are stored in the vector of pairs 
	void displayVarNValues(std::vector<std::pair<std::string, int>>& vars) {
		for (std::pair<std::string, int>& element : vars) {
			std::cout << element.first << "   ";
			std::cout << element.second << std::endl;
		}
	}


	// finds the offset of a local variable based on looking into a vector of pairs
	short getOffset(std::vector<std::pair<std::string, short>>& vars, std::string& variableName) {
		for (std::pair<std::string, short> &element : vars) {
			if (element.first == variableName) {
				return element.second;
			}

		}
		return -1;
	}

	// finds the value of a local variable based on looking into a vector of pairs
	int getValue(std::vector<std::pair<std::string, int>>& vars, std::string& variableName) {
		for (std::pair<std::string, int> &element : vars) {
			if (element.first == variableName) {
				return element.second;
			}
			
		}
		return -1;
	}

	// Function that handles the return statement it updates the output and looks for the offset of the returned value 
	void handleReturnStatement(std::vector<std::string>& answer, std::string& returnStatement, std::vector<std::pair<std::string, short>>& localVarsOffsets) {
		// parse the return statement to get its return value
		returnStatement = returnStatement.substr(returnStatement.find("n") + 2);
		returnStatement= returnStatement.substr(0, returnStatement.find(";"));
		short offset = getOffset(localVarsOffsets, returnStatement);
		if (offset == -1) { // meaning the returned value is not a variable rather a literal
			std::string sentence = "   movl $" + returnStatement;
			sentence = sentence + ", %eax";
			answer.push_back(sentence);
		}
		else {
			std::string sentence = "   movl "+std::to_string(offset);
			sentence = sentence + "(%rbp), %eax";
			answer.push_back(sentence);
		}
	}


}

