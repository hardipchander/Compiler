#include "pch.h"

#include "Action.h"

// namespace below has all the helper function definitions
namespace HelperFunc {
	// returns true or false on whether the current input line is a function header
	bool HelperFunc::isFuncHeaderInLine(std::string& line) {
		return ( (line.find("(") != -1) && (line.find(")") != -1) && (line.find("for") == -1) && (line.find(";") == -1)) ? true : false;
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

	// Returns what operator is in the line
	std::string whatOperator(std::string& operationLine) {
		if (operationLine.find("+") != -1) {
			return "+";
		}
		else if (operationLine.find("-") != -1) {
			return "-";
		}
		else if (operationLine.find("*") != -1) {
			return "*";
		}
		else {
			return "/";
		}
	}

	// Handle simple arithmetic operations +,-,*,/
	void HandleSimpleArithmetic(std::vector<std::string>& answervector, std::string& operationstatement, std::vector<std::pair<std::string, short>>& localVarsOffsets) {
		if (whatOperator(operationstatement)=="/") { //  the / operation 
			std::string c3 = operationstatement.substr(0, operationstatement.find("="));
			operationstatement = operationstatement.substr(0, operationstatement.find(";")); // remove the  ; from the end 
			operationstatement = operationstatement.substr(operationstatement.find("=") + 1);
			std::string c1 = operationstatement.substr(0, operationstatement.find(whatOperator(operationstatement)));
			operationstatement = operationstatement.substr(operationstatement.find(whatOperator(operationstatement)) + 1);
			//operationsstatement equals c3 at his point 

			// get the offsets 
			short offC3 = getOffset(localVarsOffsets, c3);
			short offC1 = getOffset(localVarsOffsets, c1);
			short offC2 = getOffset(localVarsOffsets, operationstatement);

			// Line 1
			if (offC1 == -1) { // then a literal
				answervector.push_back("   movl $" + c1 + ", %eax");
			}
			else {
				answervector.push_back("   movl " + std::to_string(offC1) + "(%rbp), %eax");
			}
			answervector.push_back("   cltd");

			// Line 2
			if (offC2 == -1) { // then a literal
				answervector.push_back("   idivl $" + operationstatement);
			}
			else {
				answervector.push_back("   idivl " + std::to_string(offC2) + "(%rbp)");
			}

			//Line 3
			answervector.push_back("   movl %eax, " + std::to_string(offC3) + "(%rbp)");
		}
		else { // other operations +, -,*
			std::string operate;
			if (whatOperator(operationstatement) == "+") {
				operate = "   addl ";
			}
			else if (whatOperator(operationstatement) == "-") {
				operate = "   subl ";
			}
			else if (whatOperator(operationstatement) == "*") {
				operate = "   imull ";
			}

			std::string c3 = operationstatement.substr(0, operationstatement.find("="));
			operationstatement = operationstatement.substr(0, operationstatement.find(";")); // remove the  ; from the end 
			operationstatement = operationstatement.substr(operationstatement.find("=")+1);
			std::string c1 = operationstatement.substr(0, operationstatement.find(whatOperator(operationstatement)));
			operationstatement = operationstatement.substr(operationstatement.find(whatOperator(operationstatement)) + 1);
			//operationsstatement equals c3 at his point 

			// get the offsets 
			short offC3 = getOffset(localVarsOffsets, c3);
			short offC1 = getOffset(localVarsOffsets, c1);
			short offC2 = getOffset(localVarsOffsets, operationstatement);

			// Line 1
			if (offC1 == -1) { // then a literal
				answervector.push_back("   movl $"+c1+", %eax");
			}
			else {
				answervector.push_back("   movl " +std::to_string(offC1) + "(%rbp), %eax");
			}

			
			// Line 2
			if (offC2 == -1) { // then a literal
				answervector.push_back(operate+"$"+operationstatement + ", %eax");
			}
			else {
				answervector.push_back(operate+ std::to_string(offC2) + "(%rbp), %eax");
			}

			//Line 3
			answervector.push_back("   movl %eax, " + std::to_string(offC3) + "(%rbp)");
		}
	}

	// Handle 1st part of For Loop
	void handleFirstPart(std::string& part, std::vector<std::string>& answervector, std::vector<std::pair<std::string, short>>& localVarsOffsets, short &off) {
		// only if in nice form int x=0 might have to add to this function in the function 
		answervector.push_back("   movl $"+part.substr(part.find("=")+1)+", "+std::to_string(off)+"(%rbp)");
		//Add variable to the variable and offset data structure 
		part = part.substr(part.find(" ") + 1);
		part = part.substr(0, part.find("=")); // get the variable name
		std::pair<std::string,short> loopvar(part,off);
		localVarsOffsets.push_back(loopvar);
		
		// Don't forget to decrement the offset variable by 4
		off = off - 4;
	}

	// Function that handles the return statement it updates the output and looks for the offset of the returned value 
	void handleReturnStatement(std::vector<std::string>& answer, std::string& returnStatement, std::vector<std::pair<std::string, short>>& localVarsOffsets) {
		// parse the return statement to get its return value
		returnStatement = returnStatement.substr(returnStatement.find("n") + 2); // get the return value
		returnStatement= returnStatement.substr(0, returnStatement.find(";"));  // remove the ; from line

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

