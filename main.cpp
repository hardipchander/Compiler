// Hardip Chander 

#include "pch.h"
#include "Action.h"

// Global Array String of Paramter Registers 
const std::string paramReg[6] = {"%edi","%esi","%edx","%ecx","%r8d","%r9d"};

// Containers that store function information 
std::vector<std::string> retTypes;
std::vector<std::string> functionNames;

// Vector of Pairs that represent the local varible names and their offsets 
std::vector<std::pair<std::string, short> > localVars;

// Vector of Pairs that represent the local variable names and their values
std::vector<std::pair<std::string, int> > varsNValues;

short offset = -4;

// The final answer that stores all the assembly instructions 
std::vector<std::string> output;

int main() {
	std::string inputCfile= "../test.c";
	std::ifstream cfile(inputCfile);
	std::string line;

	// Read the C file line by line
	while (getline(cfile, line)) {
		if (HelperFunc::isFuncHeaderInLine(line)) {  // Process the Function Header Line -------------------------------------------------------------------
			// Get the return type
			retTypes.push_back(line.substr(0, line.find(" ")));
			line=line.substr(line.find(" ") + 1);

			// Parse header line to get function name 
			functionNames.push_back(line.substr(0, line.find("(")));
			line = line.substr(line.find("(") + 1);
			line = line.substr(0, line.find(")"));
			output.push_back(functionNames[functionNames.size() - 1] + ":");
			output.push_back("   pushq %rbp");
			output.push_back("   movq %rsp, %rbp");
			
			// Get the function parameters if the function has 
			if (line.find("int") != -1) {
				// Call my Python Delimeter like function 
				std::vector<std::string> parmetersNames;
				HelperFunc::breakString(line, ',', parmetersNames);

				// Get the actual parameter variable names and store them in localVars 
				for (std::string& paramter : parmetersNames) {
					paramter = paramter.substr(paramter.find(" ") + 1);
					std::pair<std::string, short> var(paramter,offset);
					localVars.push_back(std::move(var));
					output.push_back("   movl " + paramReg[(offset / -4) - 1]+", "+std::to_string(offset) + "(%rbp)");
					offset = offset - 4;
				}

			}
		}
		else if (line.find("int")!=-1 && line.find("for") == -1 && line.find("while") == -1) {  // Process Variable declaration --------------------------------
			//parse variable declartion line 
			line = line.substr(line.find(" ") + 1);
			line = line.substr(0, line.find(";")); // remove the ; from the line 
			

			if (line.find("[") != -1 && line.find("]") != -1) {  // Array Declartion
				
			}
			else { // Variable Declartion with its variable 
				std::vector<std::string> varDeclartions;
				HelperFunc::breakString(line, ',', varDeclartions);
				for (std::string& variableDeclartion : varDeclartions) {
					// Get variable name and its value 
					std::string varName = variableDeclartion.substr(0, variableDeclartion.find("="));
					int varValue = std::stoi(variableDeclartion.substr(variableDeclartion.find("=") + 1));

					// Now store the variable name and its offset and its value into the vector of pairs 
					std::pair varoff(varName,offset);
					localVars.push_back(std::move(varoff));
					offset = offset - 4;

					std::pair varVal(varName, varValue);
					varsNValues.push_back(std::move(varVal));
					
				}
			}
		}
		else if (line.find("return")!=-1) {               // Process the Return statement and the end of the function ---------------------------------------------
			HelperFunc::handleReturnStatement(output,line,localVars);

		}
		
		
	}

	// Displaying the assembly code for the final answer 
	for (std::string& outputline : output) {
		PRINT(outputline);
	}

	return 0;
}