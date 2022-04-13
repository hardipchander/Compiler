#include "pch.h"

#include "Action.h"

// namespace below has all the helper function definitions 
namespace HelperFunc {
	// Returns true or false on whether the current input line is a function header
	bool HelperFunc::isFuncHeaderInLine(std::string& line) {
		return ( (line.find("(") != -1) && (line.find(")") != -1 ) && line.find("else") == -1 && (line.find("if") == -1 &&  line.find("for") == -1) && (line.find(";") == -1)) ? true : false;
	}

	// Returns true or false in whether the current input line is a function call 
	bool isFunctionCall(std::string& line, std::vector<std::string>& namesOfFunctions) {
		bool output = false;
		for (std::string& name : namesOfFunctions) {
			if (line.find(name)!=-1) {
				output = true;
			}
		}
		return output;
	}

	// Breaks the string based on the breaker char and stores the pieces into storage 
	void breakString(std::string& line, char breaker, std::vector<std::string>& storage) {
		std::stringstream list(line);
		std::string out;

		// Add to the storage vector one by one
		while (std::getline(list, out, breaker)) {
			storage.push_back(out);
		}
	}

	// Checks if operand is an array with an variable inside its brackets to see if conversion instructions are needed
	bool isArrWithVariable(std::string& operand, std::vector<std::pair<std::string, short>>& localVariables) {
		if (operand.find("[") != -1) {
			std::string insideBrackets = operand.substr(operand.find("[") + 1);
			insideBrackets = insideBrackets.substr(0, insideBrackets.find("]"));
			if (HelperFunc::getOffset(localVariables, insideBrackets) != -1) { // inside the operand array brackets is a variable 
				return true;
			}
			else {
				return false;
			}
		}
		else {
			return false;
		}
	}

	// displays the local variables and their offsets stored which are stored in the vector of pairs used for Debugging Purposes 
	void display(std::vector<std::pair<std::string, short>>& vars) {
		for (std::pair<std::string, short> &element : vars) {
			std::cout << element.first << "   ";
			std::cout << element.second <<std::endl;
		}
	}

	// displays the local variables and their values stored which are stored in the vector of pairs used for Debugging Purposes 
	void displayVarNValues(std::vector<std::pair<std::string, int>>& vars) {
		for (std::pair<std::string, int>& element : vars) {
			std::cout << element.first << "   ";
			std::cout << element.second << std::endl;
		}
	}


	// Finds the offset of a local variable based on looking into a vector of pairs
	short getOffset(std::vector<std::pair<std::string, short>>& vars, std::string& variableName) {
		short out = -1;
		for (std::pair<std::string, short> &element : vars) {
			if (element.first == variableName) {
				out=element.second;
			}

		}

		return out;
	}

	// Finds the value of a local variable based on looking into a vector of pairs that stores the varibale name with its value
	int getValue(std::vector<std::pair<std::string, int>>& vars, std::string& variableName) {
		short out=-1;
		for (std::pair<std::string, int> &element : vars) {
			if (element.first == variableName) {
				out=element.second;
			}
			
		}
		return out;
	}

	// Sets the value of a local variable or an array element
	void setValueOfVar(std::vector<std::pair<std::string, int>>& VarValues, std::string& varName, int newValue) {
		for (std::pair<std::string, int>& element : VarValues) {
			if (element.first == varName) {
				element.second = newValue;
			}

		}
	}

	// Convert Arrayname and Index to special format name index (arrayName[index]) ---> (Arrayname index)
	std::string convertArrName(std::string& originalName) {
		std::string ArrVarNam = originalName.substr(0, originalName.find("[")); // get b
		std::string index = originalName.substr(originalName.find("[") + 1); // get literal
		index = index.substr(0, index.find("]"));  // get literal part 2

		return ArrVarNam + " " + index;
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
	void HandleSimpleArithmetic(std::vector<std::string>& answervector, std::string& operationstatement, std::vector<std::pair<std::string, short>>& localVarsOffsets, std::vector<std::pair<std::string, int>>& VarValues, bool& operB, bool& operC) {
		if (whatOperator(operationstatement)=="/") { //  the / operation 
			std::string c3 = operationstatement.substr(0, operationstatement.find("="));
			operationstatement = operationstatement.substr(0, operationstatement.find(";")); // remove the  ; from the end 
			operationstatement = operationstatement.substr(operationstatement.find("=") + 1);
			std::string c1 = operationstatement.substr(0, operationstatement.find(whatOperator(operationstatement)));
			operationstatement = operationstatement.substr(operationstatement.find(whatOperator(operationstatement)) + 1);
			//operationsstatement equals c2 at his point 

			// get the offsets ------------------------------------
			short offC3;
			if (c3.find("[") != -1) { // c3 is an array element
				std::string realArrName = convertArrName(c3);
				offC3 = getOffset(localVarsOffsets, realArrName);
			}
			else {  // c3 is an regular variable or literal 
				offC3= getOffset(localVarsOffsets, c3);
			}

			short offC1;
			if (c1.find("[") != -1) { // c1 is an array element
				std::string realArrNa = convertArrName(c1);
				offC1 = getOffset(localVarsOffsets, realArrNa);
			}
			else {  // c1 is an regular variable or literal
				offC1 = getOffset(localVarsOffsets, c1);
			}

			short offC2;
			if (operationstatement.find("[") != -1) { // c2 is an array element
				std::string realArrN = convertArrName(operationstatement);
				offC2 = getOffset(localVarsOffsets, realArrN);
			}
			else {  // c2 is an regular variable or literal
				offC2 = getOffset(localVarsOffsets, operationstatement);
			}
			//--------------------------------------------------------------
			// real values for value in the variable and value data structure
			int valC1=-1;
			int valC2=-1;

			// Line 1
			if (offC1 == -1) { // then a literal
				answervector.push_back("   movl $" + c1 + ", %eax");
				valC1 = std::stoi(c1);
			}
			else {
				answervector.push_back("   movl " + std::to_string(offC1) + "(%rbp), %eax");
				valC1 = getValue(VarValues,c1);
			}
			answervector.push_back("   cltd");

			// Line 2
			if (offC2 == -1) { // then a literal
				answervector.push_back("   idivl $" + operationstatement);
				valC2 = std::stoi(operationstatement);
			}
			else {
				answervector.push_back("   idivl " + std::to_string(offC2) + "(%rbp)");
				valC2 = getValue(VarValues, operationstatement);
			}

			//Line 3
			answervector.push_back("   movl %eax, " + std::to_string(offC3) + "(%rbp)");

			// Updating the value in the variable and value data structure 
			if (c3.find("[") != -1) { // c3 is an array element
				std::string realArrayName = convertArrName(c3);
				setValueOfVar(VarValues, realArrayName, (valC1 / valC2));
			}
			else {   
				setValueOfVar(VarValues, c3, (valC1 / valC2));
			}

		}
		else { // other operations +, -,* --------------------------------------------------------------------------
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
			short offC3;
			if (c3.find("[") != -1) { // c3 is an array element
				std::string realArrName = convertArrName(c3);
				offC3 = getOffset(localVarsOffsets, realArrName);
			}
			else {  // c3 is an regular variable or literal 
				offC3 = getOffset(localVarsOffsets, c3);
			}

			short offC1;
			if (c1.find("[") != -1) { // c1 is an array element
				std::string realArrNa = convertArrName(c1);
				offC1 = getOffset(localVarsOffsets, realArrNa);
			}
			else {  // c1 is an regular variable or literal
				offC1 = getOffset(localVarsOffsets, c1);
			}

			short offC2;
			if (operationstatement.find("[") != -1) { // c2 is an array element
				std::string realArrN = convertArrName(operationstatement);
				offC2 = getOffset(localVarsOffsets, realArrN);
			}
			else {  // c2 is an regular variable or literal
				offC2 = getOffset(localVarsOffsets, operationstatement);
			}
			//--------------------------------------------------------------
			// real values for value in the variable and value data structure
			int valC1=-1;
			int valC2=-1;

			// Line 1
			if (operB == true) { // then operand b or c1 is array with a variable inside its brackets
				answervector.push_back("   movl %r13d, %eax");
			}
			else if (offC1 == -1) { // then a literal
				answervector.push_back("   movl $"+c1+", %eax");
				valC1 = std::stoi(c1);
			}
			else {
				answervector.push_back("   movl " +std::to_string(offC1) + "(%rbp), %eax");
				valC1 = getValue(VarValues, c1);
			}

			
			// Line 2
			if (operC == true) { // then operand c or c2 is array with a variable inside its brackets
				answervector.push_back(operate +"%r14d, %eax");
			}
			else if (offC2 == -1) { // then a literal
				answervector.push_back(operate+"$"+operationstatement + ", %eax");
				valC2 = std::stoi(operationstatement);
			}
			else {
				answervector.push_back(operate+ std::to_string(offC2) + "(%rbp), %eax");
				valC2 = getValue(VarValues, operationstatement);
			}

			//Line 3
			answervector.push_back("   movl %eax, " + std::to_string(offC3) + "(%rbp)");

			// Updating the value in the variable and value data structure 
			if (c3.find("[") != -1) { // c3 is an array element
				std::string realArrayName = convertArrName(c3);
				if (operate == "   addl ") {
					setValueOfVar(VarValues, realArrayName, (valC1+valC2));
				}
				else if(operate == "   subl ") {
					setValueOfVar(VarValues, realArrayName, (valC1-valC2));
				}
				else if (operate == "   imull ") {
					setValueOfVar(VarValues, realArrayName, (valC1*valC2));
				}
				
			}
			else {  
				if (operate == "   addl ") {
					setValueOfVar(VarValues, c3, (valC1 + valC2));
				}
				else if (operate == "   subl ") {
					setValueOfVar(VarValues, c3, (valC1 - valC2));
				}
				else if (operate == "   imull ") {
					setValueOfVar(VarValues, c3, (valC1 * valC2));
				}
			}
		}
	}

	// Handle 1st part of For Loop
	void handleFirstPart(std::string& part, std::vector<std::string>& answervector, std::vector<std::pair<std::string, short>>& localVarsOffsets, short &off, std::vector<std::pair<std::string, int>> &VarValues) {
		// only if in nice form int x=0 might have to add to this function in the future 
		answervector.push_back("   movl $"+part.substr(part.find("=")+1)+", "+std::to_string(off)+"(%rbp)");
		//Add variable to the variable and offset data structure 
		part = part.substr(part.find(" ") + 1);
		std::string name = part.substr(0, part.find("=")); // get the variable name
		std::pair<std::string,short> loopvar(name,off);
		localVarsOffsets.push_back(std::move(loopvar));

		// Add variable to the variable and value data structure
		int value = std::stoi(part.substr(part.find("=") + 1));
		std::pair<std::string, int> loopVarValue(name, value);
		VarValues.push_back(std::move(loopVarValue));

		// Don't forget to decrement the offset variable by 4
		off = off - 4;
	}

	// Handle 2nd part of For Loop and adds first two lines of assembly code under the begining loop label  
	void handleSecondPart(std::string& part, std::vector<std::string>& answervector, short& labelNum, short& off) {
		// The real loop variable offset 
		short offsetOfLoopVar = off + 4;

		// Getting the opposite comparsion and adding 1st two lines of assembly code under the loop label
		if (part.find("<=") != -1) {
			answervector.push_back("   cmpl $"+(part.substr(part.find("=")+1))+", "+std::to_string(offsetOfLoopVar)+"(%rbp)");
			answervector.push_back("   jg .L" + std::to_string(labelNum) + "Exit:");
		}
		else if (part.find(">=") != -1) {
			answervector.push_back("   cmpl $" + (part.substr(part.find("=") + 1)) + ", " + std::to_string(offsetOfLoopVar) + "(%rbp)");
			answervector.push_back("   jl .L" + std::to_string(labelNum) + "Exit:");
		}
		else if (part.find(">") != -1) {
			answervector.push_back("   cmpl $" + (part.substr(part.find(">") + 1)) + ", " + std::to_string(offsetOfLoopVar) + "(%rbp)");
			answervector.push_back("   jle .L" + std::to_string(labelNum) + "Exit:");
		}
		else if (part.find("<") != -1) {
			answervector.push_back("   cmpl $" + (part.substr(part.find("<") + 1)) + ", " + std::to_string(offsetOfLoopVar) + "(%rbp)");
			answervector.push_back("   jge .L" + std::to_string(labelNum) + "Exit:");
		}
		else if (part.find("==") != -1) {
			answervector.push_back("   cmpl $" + (part.substr(part.find("=") + 1)) + ", " + std::to_string(offsetOfLoopVar) + "(%rbp)");
			answervector.push_back("   jne .L" + std::to_string(labelNum) + "Exit:");
		}
		else if (part.find("!=") != -1) {
			answervector.push_back("   cmpl $" + (part.substr(part.find("=") + 1)) + ", " + std::to_string(offsetOfLoopVar) + "(%rbp)");
			answervector.push_back("   je .L" + std::to_string(labelNum) + "Exit:");
		}
	}

	// Handle 3nd part of For Loop and adds the increment value that changes the value of the for loop variable 
	void handleThirdPart(std::string& part, std::vector<std::string>& answervector, short& off) {
		// might have to update loop variable in the future  !!!!!!!!!!!!!!!!!!!!!!!!!!
		if (part.find("++") != -1) {
			answervector.push_back("   addl $1, " + std::to_string(off) + "(%rbp)");
		}
		else if (part.find("--") != -1) {
			answervector.push_back("   subl $1, " + std::to_string(off) + "(%rbp)");
		}
		else if (part.find("=") != -1 && part.find("+") != -1) {
			answervector.push_back("   addl $"+(part.substr(part.find("+") + 1))+", "+ std::to_string(off) + "(%rbp)");
		}
		else if (part.find("=") != -1 && part.find("-") != -1) {
			answervector.push_back("   subl $"+ (part.substr(part.find("-") + 1))+", "+ std::to_string(off) + "(%rbp)");
		}
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

