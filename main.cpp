// Hardip Chander 

#include "pch.h"
#include "Action.h"

// Global Array String of Paramter Registers 
const std::string paramReg[6] = {"edi","esi","edx","ecx","r8d","r9d"};

// Containers that store function information 
std::vector<std::string> retTypes;
std::vector<std::string> functionNames;

// Vector of Pairs that represent the local varible names and their offsets 
std::vector<std::pair<std::string, int> > localVars;

const int counter = -4;

// The final answer
std::vector<std::string> output;

int main() {
	

	std::string inputCfile= "../test.c";
	std::ifstream cfile(inputCfile);
	std::string line;

	// Read the C file line by line
	while (getline(cfile, line)) {
		
		// Process the Function Header Line 
		if (HelperFunc::isFuncHeaderInLine(line)) {
			// Get the return type
			retTypes.push_back(line.substr(0, line.find(" ")));
			line=line.substr(line.find(" ") + 1);

			// Parse header line to get function name 
			functionNames.push_back(line.substr(0, line.find("(")));
			line = line.substr(line.find("(") + 1);
			line = line.substr(0, line.find(")"));
			output.push_back(functionNames[functionNames.size() - 1] + ":");
			output.push_back("  pushq %rbp");
			output.push_back("  movq %rsp, %rbp");
			
			// Get the function parameters if the fucntion has 
			if (line.find("int") != -1) {
				// Call my Python Delimeter like function 
				std::vector<std::string> parmetersNames;
				HelperFunc::breakString(line, ',', parmetersNames);

				// Get the actual parmater vairble names
				for (std::string& paramter : parmetersNames) {
					paramter = paramter.substr(paramter.find(" ") + 1);
					
				}

			}
		}
		
	}

	// Displaying the assembly code for the final answer 
	for (std::string& outputline : output) {
		std::cout << outputline << std::endl;
	}

	return 0;
}