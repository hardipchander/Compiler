// Hardip Chander 

#include "pch.h"
#include "Action.h"

// Global Arrays String of Parameter Registers 
const std::string paramReg[6] = {"%edi","%esi","%edx","%ecx","%r8d","%r9d"};
const std::string paramRegEightBytes[6] = {"%rdi","%rsi","%rdx","%rcx","%r8","%r9"};

// Short that will used to generate label names 
short labelNumber = 1;

// Bool that tells me if lines are in a for loop
bool inForLoop = false;

// Bool that tells me if lines are in a if clause 
bool inIfStatement = false;

// Bool that tells me whether the main has a return statement at the end of it 
bool mainHasReturnStatement=false;

// Bool that tells me if lines are in a else clause
bool inElseStatement = false;

// Third part of the for loop statement and the original offset of for loop variable 
std::string thirdLoopPart;
short originalOffset;

// Containers that store function information like the function names and their return types 
std::vector<std::string> retTypes;
std::vector<std::string> functionNames;

// Vector of Pairs that represent the local variable names and their offsets and each pair represents a variable with its name and offset
std::vector<std::pair<std::string, short> > localVars;

// Vector of Pairs that represent the local variable names and their values and each pair represents a variable with its name and value
std::vector<std::pair<std::string, int> > varsNValues;

// Offset for the all the local variables, these variables will be modified throughout the program as variables are created 
short offset = -4;
short offsetForParameter7th = 16;

// The final answer that stores all the assembly instructions and at the end of program I will print all the elements of output
std::vector<std::string> output;

int main() {
	// in inputCfile string place the name of the c file in the format of (../cfile)
	std::string inputCfile= "../TestCase2.c";
	std::ifstream cfile(inputCfile);
	std::string line;

	// Read the C file line by line
	while (getline(cfile, line)) {
		// Changing x++; and x--; statements for later code to handle them 
		if (line.find("++;")!=-1) {
			// change x++; to x=x+1;
			line = (line.substr(0, line.find("++;"))) + "=" + (line.substr(0, line.find("++;"))) + "+1;";
		}
		else if (line.find("--;")!=-1) {
			// change x--; to x=x-1;
			line = (line.substr(0, line.find("--;"))) + "=" + (line.substr(0, line.find("--;"))) + "-1;";
		}

		if (HelperFunc::isFuncHeaderInLine(line)) {  // Process the Function Header Line -------------------------------------------------------------------
			// print a space line between functions , used for display on terminal 
			if (offset != -4) {
				output.push_back(" ");
			}
			
			// reset the offset to -4 at the top of each function 
			offset = -4;

			// Reset the special offset for the 7th, 8th .... parameters 
			offsetForParameter7th = 16;

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
			
			// modifying rsp if main is not a leaf function 
			if (functionNames[functionNames.size() - 1] == "main" && (functionNames.size() > 1)) {
				output.push_back("   subq $32, %rsp");
			}

			// Get the function parameters if the function has 
			if (line.find("int") != -1) {
				// Call the Python Delimeter like function 
				std::vector<std::string> parametersNames;
				HelperFunc::breakString(line, ',', parametersNames);

				// Get the actual parameter variable names and store them in localVars 
				for (int j = 0; j < parametersNames.size(); j++) {
					std::string parameter = parametersNames[j];
					parameter = parameter.substr(parameter.find(" ") + 1);

					// For the first six parameters of the function 
					if (j < 6) { 
						if (parameter.find("[") == -1) { // regular int parameter 
							std::pair<std::string, short> var(parameter, offset);
							localVars.push_back(std::move(var));
							output.push_back("   movl " + paramReg[j] + ", " + std::to_string(offset) + "(%rbp)");
							offset = offset - 4;
						}
						else { // parameter is an array 
							offset = offset - 4; // create larger space for array parameter 
							std::pair<std::string, short> var(parameter, offset);
							localVars.push_back(std::move(var));

							// Array Base address is stored 
							output.push_back("   movq " + paramRegEightBytes[j] + ", " + std::to_string(offset) + "(%rbp)");
							offset = offset - 4;
						}

					}
					else {  // for the rest of the parameters 7th and beyond  
						if (parameter.find("[") == -1) { // regular int parameter
							std::pair<std::string, short> var(parameter, offsetForParameter7th);
							localVars.push_back(std::move(var));
							offsetForParameter7th = offsetForParameter7th + 8; // above rbp 
						}
						else { // parameter is an array 
							// Put the Base Address of Array above rbp when it is 7th parameter or the 8th parameter ....
							std::pair<std::string, short> var(parameter, offsetForParameter7th);
							localVars.push_back(std::move(var));
							offsetForParameter7th = offsetForParameter7th + 8; // above rbp 
						}
					}
				
				} 
				
			}
		}
		else if (HelperFunc::isFunctionCall(line,functionNames)==true) {    //Process Function Call in main function ---------------------------------------------------------------------------------------
			// Determine whether the function has a return type and Getting the function name
			std::string funcName;
			std::string variableModifiedByFunction = "-1";  // -1 meaning that the function does not have a return type 
			if (line.find("=") != -1) { // function call has a return type
				funcName = line.substr(line.find("=") + 1);
				funcName = funcName.substr(0, funcName.find("("));
				variableModifiedByFunction = line.substr(0, line.find("="));
			}
			else { // No return type so function call is just FunctionName(a,b,c);
				funcName = line.substr(0, line.find("("));
			}

			// Handle the Parameters Now 
			line = line.substr(line.find("(") + 1);
			line = line.substr(0, line.find(")")); // remove the ( ); parts 

			// Break parameters into a vector
			std::vector<std::string> parameters;
			std::vector<std::string> parameters7thandMore;
			HelperFunc::breakString(line, ',', parameters);
			
			// Examinning each parameter in the function call one by one 
			for (int u = 0; u < parameters.size(); u++) {
				std::string param = parameters[u];

				if (u < 6) { // the first six parameters 
					// check if parameter is an array first 
					std::string Arrparam = param + " 0";
					if (HelperFunc::getOffset(localVars, Arrparam) != -1) { // then array parameter 
						output.push_back("   leaq "+std::to_string(HelperFunc::getOffset(localVars, Arrparam))+ "(rbp), "+paramRegEightBytes[u]);
					}
					else { // normal integer variable parameter
						output.push_back("   movl " + std::to_string(HelperFunc::getOffset(localVars, param)) + "(%rbp), " + paramReg[u]);
					}

				}
				else { // the 7th and beyond parameters 
					// store them in a different vector for reverse order  
					parameters7thandMore.push_back(std::move(param));
				}
			}
			// Handling 7th and more parameters in the separate vector parameters7thandMore in reverse order
			for (int y = parameters7thandMore.size() - 1; y >= 0; y--) { // reverse order
				std::string param7or8or9 = parameters7thandMore[y];

				// check if parameter is an array first 
				std::string Arrparameter = param7or8or9 + " 0";
				if (HelperFunc::getOffset(localVars, Arrparameter) != -1) { // then array parameter 
					// Use 64 bit register %rbx to store the base address of the array and then push onto stack the value of rbx 
					output.push_back("   leaq "+ std::to_string(HelperFunc::getOffset(localVars, Arrparameter))+"(%rbp), %rbx");
					output.push_back("   pushq %rbx");
				}
				else { // normal integer variable parameter
					// Use 32 bit register %r10d to store parameter value and then push it onto the stack 
					output.push_back("   pushq $" + std::to_string(HelperFunc::getValue(varsNValues,param7or8or9)));
					
				}
			}

			// Calling function in Assembly 
			output.push_back("   call "+funcName);

			// addq after the function call if the function has more than 6 parameters 
			if (parameters.size() > 6) {
				int addqOffset = (parameters7thandMore.size())*8;
				output.push_back("   addq $"+std::to_string(addqOffset)+", %rsp");
			}

			// storing %eax into variableModifiedByFunction the return type
			if (variableModifiedByFunction!="-1") { // function has a return type 
				output.push_back("   movl %eax, " + std::to_string(HelperFunc::getOffset(localVars,variableModifiedByFunction)) + "(%rbp)");
			}

		}
		else if (line.find("int")!=-1 && line.find("for") == -1 && (line.find("else") == -1 && line.find("if") == -1)) {  // Process Variable declaration --------------------------------------------------------
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
					int varValue;

					//Need to check if the value after the equal sign is a int literal or a actual local variable !!!!!!!!!
					std::string valueAfterEqualSign = variableDeclartion.substr(variableDeclartion.find("=") + 1);
					if (HelperFunc::getOffset(localVars, valueAfterEqualSign)== -1) { // then the value after the equal sign is integer literal 
						varValue = std::stoi(valueAfterEqualSign);

						// Now store the variable name and its offset and its value into the vector of pairs 
						std::pair varoff(varName, offset);
						localVars.push_back(std::move(varoff));

						std::pair varVal(varName, varValue);
						varsNValues.push_back(std::move(varVal));

						// Add assembly instruction by updating output for variable declaration statement 
						std::string outline = "   movl $" + std::to_string(varValue);
						outline = outline + ", " + std::to_string(offset);
						outline = outline + "(%rbp)";
						output.push_back(std::move(outline));
					}
					else { // then the value after the equal sign is a local variable 
						// Now store the variable name and its offset into the vector of pairs 
						std::pair varoff(varName, offset);
						localVars.push_back(std::move(varoff));

						// Add assembly instruction
						short offOfVarAfterEqualSign = HelperFunc::getOffset(localVars, valueAfterEqualSign);
						// store value of local variable after the equal sign into 32 bit register ebx
						std::string firstLine = "   movl " +std::to_string(offOfVarAfterEqualSign)+ "(%rbp), %ebx";
						output.push_back(firstLine);
						output.push_back("   movl %ebx, "+ std::to_string(offset)+"(%rbp)");
					}

					// Update the offset for the next potential local variable 
					offset = offset - 4;
				}
			}
		}  //Process Arithemtic statements simple ( a=b(+/-/*//)c) -----------------------------------------------------------------------------------------------------------------
		else if(line.find("int")==-1 && line.find("=") != -1 && (line.find("else") == -1 && line.find("if") == -1) &&(line.find("+") != -1 || line.find("-") != -1 || line.find("*") != -1) || line.find("/") != -1) {
			// Check if b and c are array operands that have a variable inside brackets then I need to add conversion cltq instructions
			std::string copyOfLine = line;
			copyOfLine = copyOfLine.substr(copyOfLine.find("=") + 1); // get b (+/-/*//)c; form 
			copyOfLine = copyOfLine.substr(0, copyOfLine.find(";"));  //  remove the ; 
			
			// Flags to tell me if converions instructions are neeeded for each operand 
			bool operandBNeedConversion = false;
			bool operandCNeedConversion = false;

			// Getting b and c operands themselves 
			std::string operatorOfLine = HelperFunc::whatOperator(copyOfLine); // getting +, -,*, /
			std::string operandB = copyOfLine.substr(0, copyOfLine.find(operatorOfLine));
			std::string operandC = copyOfLine.substr(copyOfLine.find(operatorOfLine)+1);

			// Checking them for converison instructions
			if (HelperFunc::isArrWithVariable(operandB, localVars) == true) { // Operand B is an array with variable inside the brackets
				operandBNeedConversion = true;
				// I am going to move value of operand B into 32 bit register %r13d
				std::string arrName = operandB.substr(0, operandB.find("["));
				arrName = arrName + " 0";
				std::string insideBrackets = operandB.substr(operandB.find("[") + 1); 
				insideBrackets = insideBrackets.substr(0, insideBrackets.find("]"));

				// Conversion Instructions for Operand B
				output.push_back("   movl "+std::to_string(HelperFunc::getOffset(localVars,insideBrackets))+"(%rbp), %eax");
				output.push_back("   cltq");
				output.push_back("   movl "+ std::to_string(HelperFunc::getOffset(localVars, arrName))+"(%rbp,%rax,4) %eax");
				output.push_back("   movl %eax, %r13d");
			}
			if (HelperFunc::isArrWithVariable(operandC, localVars) == true) { // Operand C is an array with variable inside the brackets
				operandCNeedConversion = true;
				// I am going to move value of operand C into 32 bit register %r14d
				std::string arrName = operandC.substr(0, operandC.find("["));
				arrName = arrName + " 0";
				std::string insideBrackets = operandC.substr(operandC.find("[") + 1);
				insideBrackets = insideBrackets.substr(0, insideBrackets.find("]"));

				// Conversion Instructions for Operand C
				output.push_back("   movl " + std::to_string(HelperFunc::getOffset(localVars, insideBrackets)) + "(%rbp), %eax");
				output.push_back("   cltq");
				output.push_back("   movl " + std::to_string(HelperFunc::getOffset(localVars, arrName)) + "(%rbp,%rax,4) %eax");
				output.push_back("   movl %eax, %r14d");
			} 

			// Call helper function to handle Arithemtic statements 
			HelperFunc::HandleSimpleArithmetic(output, line, localVars, varsNValues, operandBNeedConversion, operandCNeedConversion);

		} // Handle Variable Assigment ---------------------------------------------------------------------------------------------------------------------------
		else if (line.find("int") == -1 && line.find("=") != -1 && (line.find("+") == -1 && line.find("-") == -1 && line.find("*") == -1) && line.find("/") == -1 && (line.find("else") == -1 && line.find("if") == -1)) {
			// (a=b)!!!!!! a can be variable or array element and the same with b
			std::string partBeforeEqual = line.substr(0, line.find("=")); // get a=
			std::string partAfterEqual = line.substr(line.find("=")+1);   // get b;
			partAfterEqual = partAfterEqual.substr(0, partAfterEqual.find(";"));  // get b remove the ; from b;
			
			// offset of variable a and variable b
			short offOfAVar;
			short offOfBVar;

			if (partBeforeEqual.find("[") != -1) { // a is an array element a[literal] !!!!!!! might have to come back here 
				std::string realName= HelperFunc::convertArrName(partBeforeEqual); // the format I have array elements stored in data structure 
				offOfAVar = HelperFunc::getOffset(localVars, realName);
			}
			else { // a is an regular variable 
				offOfAVar = HelperFunc::getOffset(localVars, partBeforeEqual);
			}

			if (partAfterEqual.find("[") != -1) { // b is an array element b[literal] !!!!!!! might have to come back here 
				std::string realNam = HelperFunc::convertArrName(partAfterEqual); // the format I have array elements stored in data structure 
				offOfBVar = HelperFunc::getOffset(localVars, realNam);
			}
			else if (HelperFunc::getOffset(localVars, partAfterEqual)!=-1) { // b is an regular variable
				offOfBVar = HelperFunc::getOffset(localVars, partAfterEqual);
			}
			else { // b is a literal
				offOfBVar = 10; // this will tell me that in (a=b) that b is a literal
			}


			// Addind to the output 
			if (offOfBVar == 10) { // in (a=b) that b is a literal
				output.push_back("   movl $"+partAfterEqual+", "+std::to_string(offOfAVar)+"(%rbp)");
			}
			else {
				output.push_back("   movl " + std::to_string(offOfBVar) + "(%rbp)" + ", %eax");
				output.push_back("   movl %eax, "+std::to_string(offOfAVar) + "(%rbp)");
			}

		}
		else if (line.find("if")!=-1) {  //Process if statments or if-else statements ----------------------------------------------------------------------------------------------
			inIfStatement = true; // set the flag to true 

			line = line.substr(line.find("(") + 1); // remove if( ) {
			line = line.substr(0, line.find(")"));  // get a<b
			std::string inverseLogic; // store the opposite conditional jump like > - jle 
			std::string comparsion; // store the comparsion operator itself 

			// Handling the comparsion  might have to come here again for check !!!!!!!!!!!!!!!!!!!!!
			if (line.find("<=") != -1) {
				inverseLogic = "   jg ";
				comparsion = "<=";
			}
			else if (line.find(">=") != -1) {
				inverseLogic = "   jl ";
				comparsion = ">=";
			}
			else if (line.find(">") != -1) {
				inverseLogic = "   jle ";
				comparsion = ">";
			}
			else if (line.find("<") != -1) {
				inverseLogic = "   jge ";
				comparsion = "<";
			}
			else if (line.find("==") != -1) {
				inverseLogic = "   jne ";
				comparsion = "==";
			}
			else if (line.find("!=") != -1) {
				inverseLogic = "   je ";
				comparsion = "!=";
			}

			// time to get variables and their offsets
			std::string a = line.substr(0, line.find(comparsion));
			std::string b;
			short offIfA;
			short offIfB;

			if (comparsion.length() > 1) {
				b=line.substr(line.find(comparsion) + 2);
			}
			else {
				b=line.substr(line.find(comparsion)+1);
			}
			
			// have to check if a and b are array elements or literal or regular variables
			if (a.find("[") != -1) { // a is an array element
				std::string realNameofA = HelperFunc::convertArrName(a);
				offIfA = HelperFunc::getOffset(localVars, realNameofA);
			}
			else {   // a is an regular variable 
				offIfA = HelperFunc::getOffset(localVars, a);
			}

			if (b.find("[") != -1) { // b is an array element
				std::string realNameofB = HelperFunc::convertArrName(b);
				offIfB = HelperFunc::getOffset(localVars, realNameofB);
			}
			else if(HelperFunc::getOffset(localVars, b)==-1) {   // b is a literal 
				offIfB=-10; //signal that b is a literal
			}
			else { // b is a regular variable  
				offIfB = HelperFunc::getOffset(localVars, b);
			}

			// add to output
			output.push_back("   movl "+std::to_string(offIfA)+"(%rbp), %ebx");
			if (offIfB == -10) { //  b is a literal
				output.push_back("   cmpl $" +b+ ", %ebx");
			}
			else { //  b is a regular variable or array element 
				output.push_back("   cmpl " +std::to_string(offIfB)+ "(%rbp), %ebx");
			}

			// Add inverse logic jg, jne or other and the label 
			output.push_back((std::move(inverseLogic))+"L"+std::to_string(labelNumber)+":");

			// Now from here on out if body instructions 
		}
		else if (inIfStatement==true && line.find("}")!=-1 && line.find("else") != -1) { // then  a if-else structure ---------------------
			inElseStatement = true;
			output.push_back("   jmp  L" + std::to_string(labelNumber) + "Exit:"); // adding the labels 
			output.push_back("L" + std::to_string(labelNumber) + ":");  // adding the labels 
			inIfStatement = false; // set flag to false 

		}
		else if (inElseStatement==true && inIfStatement==false && line.find("}") != -1) { // at the end of the else clause 
			output.push_back("L"+std::to_string(labelNumber) + "Exit:");
			inElseStatement = false; // set flag to false
			labelNumber++; // increment at the end of else clause so can generate new labels for the future 
		}
		else if (inIfStatement == true && line.find("}") != -1) { // only a if structure --------------------------------------------------
			output.push_back("L" + std::to_string(labelNumber) + ":");
			inIfStatement = false; // set flag to false since the statement is over 
			labelNumber++;   // increment the label number to generate new label in the future 
		}
		else if (line.find("for")!=-1 && (line.find("else") == -1 && line.find("if") == -1) ) {  //Process For Loop Statement --------------------------------------------------------------------------------------------
			// Set Flag to true
			inForLoop = true;

			// remove ending ) { from line and remove for ( from line
			line = line.substr(0, line.find(")")); 
			line = line.substr(line.find("(") + 1); 
			
			// Break For loop into 3 parts and store the parts in loopParts
			std::vector<std::string> loopParts;
			HelperFunc::breakString(line, ';', loopParts);

			// Calling helper function to handle 1st part of for loop out of the three parts 
			HelperFunc::handleFirstPart(loopParts[0], output, localVars, offset, varsNValues);
			output.push_back(".L" + std::to_string(labelNumber) + ":");  // add the begining label 

			// Calling helper function to handle 2nd part of for loop to add first two lines of assembly code under the begining loop label   
			HelperFunc::handleSecondPart(loopParts[1], output, labelNumber, offset);
			
			// Store 3th loop part in global string variable and offset of for loop variable because I will need it later in different else chain 
			thirdLoopPart = loopParts[2];
			originalOffset = offset + 4;

			// From here on the rest of the code will be for loop body !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		}
		else if (line.find("}") != -1 && inForLoop == true) { // Process the end of the for loop ------------------------------------------------------------------
			// Call helper function for the 3th part of foor loop and add assembly line that increments the value of for loop variable  
			HelperFunc::handleThirdPart(thirdLoopPart,output,originalOffset);

			// Unconditional Jump label 	
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
			// Checking if the main has a return statement and if it does not then I have to manually add the stack deallocation assembly code 
			if (functionNames[functionNames.size() - 1] == "main") {
				mainHasReturnStatement = true;
			}											  
			// Helper function that processes the return statement 
			HelperFunc::handleReturnStatement(output,line,localVars);

			// Check if at the end of the main function 
			if (functionNames[functionNames.size() - 1] == "main" && (functionNames.size()>1)) {
				output.push_back("   leave");
			}
			else {
				output.push_back("   popq %rbp");
			}
			output.push_back("   ret");
		}
		
		
	}

	// if the main does not have a return statement then I have to manually add the stack deallocation assembly code leave and popq 
	if (mainHasReturnStatement == false) {
		// Check if at the end of the main function 
		if (functionNames[functionNames.size() - 1] == "main" && (functionNames.size() > 1)) {
			output.push_back("   leave");
		}
		else {
			output.push_back("   popq %rbp");
		}
		output.push_back("   ret");
	}

	// Displaying the assembly code for the final answer 
	for (std::string& line : output) {
		PRINT(line);
	}

	return 0;
}