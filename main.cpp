// Hardip Chander 

#include "pch.h"
#include "Action.h"

// Global Array String of Paramter Registers 
const std::string paramReg[6] = {"%edi","%esi","%edx","%ecx","%r8d","%r9d"};

// Short that will used to generate label names 
short labelNumber = 1;

// Bool that tells me if lines are in a for loop
bool inForLoop = false;

// Third part of the for loop statement and the oringinal offset of for loop variable 
std::string thirdLoopPart;
short originalOffset;

// Containers that store function information 
std::vector<std::string> retTypes;
std::vector<std::string> functionNames;

// Vector of Pairs that represent the local variable names and their offsets 
std::vector<std::pair<std::string, short> > localVars;

// Vector of Pairs that represent the local variable names and their values
std::vector<std::pair<std::string, int> > varsNValues;

// Offset for the all the local variables 
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
			// print a space line between functions 
			if (offset != -4) {
				output.push_back(" ");
			}
			// reset the offset to -4 at the top of each function 
			offset = -4;

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
		else if (line.find("int")!=-1 && line.find("for") == -1) {  // Process Variable declaration --------------------------------
			//parse variable declartion line 
			line = line.substr(line.find(" ") + 1); // jump to after int
			line = line.substr(0, line.find(";")); // remove the ; from the line 
			

			if (line.find("[") != -1 && line.find("]") != -1) {  // Array Declartion
				//get the name of array 1st and then its length
				std::string arrayName = line.substr(0, line.find("["));
				line = line.substr(line.find("[")+1);
				short lenOfArr = std::stoi(line.substr(0, line.find("]")));
				
				// Now process the values of the array and add them to data structure that stores offsets and values one by one
				line = line.substr(line.find("{") + 1);
				line = line.substr(0, line.find("}")); // remove the }; from end of line
				std::vector<std::string> arrValues;
				HelperFunc::breakString(line,',',arrValues);

				for (short i = 0; i < lenOfArr; i++) {
					// 1st element of the array
					if (i == 0) { 
						std::pair firstArrElement(arrayName + " " + std::to_string(i),offset);
						
						localVars.push_back(std::move(firstArrElement));
						std::pair firstElement(arrayName + " " + std::to_string(i),std::stoi(arrValues[i]));
						varsNValues.push_back(std::move(firstElement));

						// Updating output and adding the assembly instructions for each array element 
						std::string outline = "   movl $" +arrValues[i];
						outline = outline + ", "+std::to_string(offset);
						outline = outline + "(%rbp)";
						output.push_back(std::move(outline));
						offset = offset - 4;
					}
					else { // other elements of arrray  
						std::pair firstArrElement(arrayName+" "+ std::to_string(i), offset);
						localVars.push_back(std::move(firstArrElement));
						std::pair firstElement(arrayName+" "+ std::to_string(i), std::stoi(arrValues[i]));
						varsNValues.push_back(std::move(firstElement));

						// Updating output and adding the assembly instructions for each array element
						std::string outline = "   movl $" +arrValues[i];
						outline = outline + ", " + std::to_string(offset);
						outline = outline + "(%rbp)";
						output.push_back(std::move(outline));
						offset = offset - 4;
					}
				}

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

					std::pair varVal(varName, varValue);
					varsNValues.push_back(std::move(varVal));

					// Add assembly instruction by updating output for varibale declaration statement 
					std::string outline = "   movl $" +std::to_string(varValue);
					outline = outline + ", " + std::to_string(offset);
					outline = outline + "(%rbp)";
					output.push_back(std::move(outline));

					offset = offset - 4;
				}
			}
		}  //Process Arithemtic statements simple -----------------------------------------------------------------------------------------------------------------
		else if(line.find("int")==-1 && line.find("=") != -1 && (line.find("+") != -1 || line.find("-") != -1 || line.find("*") != -1) || line.find("/") != -1) {	
			// Call helper function to handle Arithemtic statements 
			HelperFunc::HandleSimpleArithmetic(output, line, localVars);
		}
		else if (line.find("for")!=-1) {  //Process For Loop Statement --------------------------------------------------------------------------------------------
			// Set Flag to true
			inForLoop = true;

			// remove ending ) { from line and remove for ( from line
			line = line.substr(0, line.find(")")); 
			line = line.substr(line.find("(") + 1); 
			
			// Break For loop into 3 parts and store the parts in loopParts
			std::vector<std::string> loopParts;
			HelperFunc::breakString(line, ';', loopParts);

			// Calling helper function to handle 1st part of for loop out of the three parts 
			HelperFunc::handleFirstPart(loopParts[0], output, localVars, offset);
			output.push_back(".L" + std::to_string(labelNumber) + ":");  // add the begining label 

			// Calling helper function to handle 2nd part of for loop to add first two lines of assembly code under the begining loop label   
			HelperFunc::handleSecondPart(loopParts[1], output, labelNumber, offset);
			
			// Store 3th loop part in global string variable and offset of for loop variable because I will need it later in different else chain 
			thirdLoopPart = loopParts[2];
			originalOffset = offset + 4;

			// From here on the rest of the code will be for loop body !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		}
		else if (line.find("}") != -1 && inForLoop == true) { // Process the end of the for loop ------------------------------------------------------------------
			// Call helper function for the 3th part of foor loop and add assembly line that incrments the value of for loop variable  
			HelperFunc::handleThirdPart(thirdLoopPart,output,originalOffset);

			// Uncondtional Jump label 	
			output.push_back("   jmp .L" + std::to_string(labelNumber) + ":");

			// end label 
			output.push_back(".L" + std::to_string(labelNumber) + "Exit:");
			labelNumber++;
			inForLoop = false;  // because I am the end of the for loop
			
			// reset global variables that keep track of the for loop
			thirdLoopPart = "";
			originalOffset = 0;

		}
		else if (line.find("return")!=-1) {               // Process the Return statement and the end of the function ---------------------------------------------
			// Helper function that processes the return statement 
			HelperFunc::handleReturnStatement(output,line,localVars);
			if (functionNames[functionNames.size() - 1] == "main" && (functionNames.size()>1)) {
				output.push_back("   leave");
			}
			else {
				output.push_back("   popq %rbp");
			}
			output.push_back("   ret");
		}
		
		
	}

	// Displaying the assembly code for the final answer 
	for (std::string& outputline : output) {
		PRINT(outputline);
	}
	
	

	return 0;
}