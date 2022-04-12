#pragma once
#include "pch.h"     // include the precompiled header 

// Make a macro for printing out things it is used for Debugging Purposes 
#define PRINT(x) std::cout<<x<<std::endl;

// namespace below has all the helper functions that I need to convert C instructions to assembly 
namespace HelperFunc {

	/** Returns true or false on whether the current input line is a function header
	//  @pre line is a line from the c file that could be instruction  
	//  @post line is not modified
	//  @param line is a line from the c file that is being compiled 
	//  @return true if line is a function header and false if it is not a function header
	*/ 
	bool isFuncHeaderInLine(std::string& line);
	
	/** Returns true or false on whether the current input line is a function call 
	//  @pre line is a line from the c file that could be instruction 
	//  @post line is not modified and namesOfFunctions is not modified 
	//  @param line is a line from the c file that is being compiled
	//  @param namesOfFunctions is vector of strings that stores the names of the functions in the c file 
	//  @return true if line is a function call and false if it is not a function call 
	*/
	bool isFunctionCall(std::string& line, std::vector<std::string>& namesOfFunctions);

	/** Breaks the string based on the breaker char and stores the pieces into storage and 
	//  It acts like the python Delimeter function VERY IMPORTANT FUNCTION !!!!!!!!!!!!!!!!
	//  @pre line is a line from the c file that could be instruction
	//  @post storage will be modified but line and breaker will not be modified 
	//  @param line is is a line from the c file that is being compiled
	//  @param breaker is a char that will be used to spilt the string into pieces and then placed into storage vector 
	//  @param storage stores all the broken string pieces 
	//  @return nothing but storage will be modified to hold the string line broken into pieces and separated based on the char breaker 
	*/
	void breakString(std::string& line, const char breaker, std::vector<std::string>& storage);

	/** Checks if operand is an array with an variable inside its brackets to see if conversion instructions are needed
	//  @pre  operand is in the form of arrayName[something] and localVariables has all the variable with their respective offsets
	//  @post operand and localVariables are not modified 
	//  @param operand is an array in the form arrayName[something] and if something is a variable then converion instruction(cltq) will be needed 
	//  @param localVariables has all the local vairables and their offsets 
	//  @return true if operand is an array with a variable inside the brackets and false otherwise 
	*/
	bool isArrWithVariable(std::string &operand, std::vector<std::pair<std::string, short> >& localVariables);

	/** Displays the local variables and their offsets which are stored in the vector of pairs 
	//  @pre It is used for debugging purposes 
	//  @post  vars is not modified 
	//  @param vars is the container that stores all the local variables in the c file and their offsets 
	//  @return nothing but it displays onto the terminal the name of the variable and right next to it the offset 
	*/
	void display(std::vector<std::pair<std::string, short> > & vars);

	/** Displays the local variables and their values which are stored in the vector of pairs
	//  @pre It is used for debugging purposes
	//  @post  vars is not modified
	//  @param vars is the container that stores all the local variables in the c file and their values 
	//  @return nothing but it displays onto the terminal the name of the variable and right next to it the value
	*/
	void displayVarNValues(std::vector<std::pair<std::string, int> >& vars);

	/** Finds the offset of a local variable based on looking into a vector of pairs
	//  @pre variableName stores the name of a local variable 
	//  @post variableName and vars is not modified 
	//  @param vars is the container that stores all the local variables in the c file and their offsets 
	//  @param variableName is the name of the local variable 
	//  @return the offset of the local variable shown in variableName by transversing through vars 
	*/
	short getOffset(std::vector<std::pair<std::string, short> >& vars, std::string& variableName);

	/** Finds the value of a local variable based on looking into a vector of pairs
	//  @pre variableName stores the name of a local variable
	//  @post variableName and vars is not modified
	//  @param vars is the container that stores all the local variables in the c file and their values 
	//  @param variableName is the name of the local variable
	//  @return the value of the local variable shown in variableName by transversing through vars
	*/
	int getValue(std::vector<std::pair<std::string, int> >& vars, std::string &variableName);

	/** Sets the value of a local variable or an array element
	//  @pre varName has the name of the variable that needs to be modified and changed to newValue 
	//  @post VarValues will be modified but varName and newValue will not be changed 
	//  @param VarValues is the container that stores all the local variables in the c file and their values 
	//  @param varName is the name of the local variable 
	//  @param newValue is the newValue that will be assigned to the variable varName
	//  @return nothing but the value of varName variable will be updated in the container VarValues 
	*/
	void setValueOfVar(std::vector<std::pair<std::string, int>>& VarValues, std::string & varName, int newValue);

	/** Convert Arrayname and Index to special format ---> (arrayName index) from the form (arrayName[index])
	//  @pre originalName is an array with brackets and a int literal inside it 
	//  @post originalName will not be modified 
	//  @param originalName is an array with brackets and a int literal inside it 
	//  @return a string with the special format of (arrayName index) and this format will be used to store it in containers 
	*/
	std::string convertArrName(std::string & originalName);

	/**  Returns what operator is in the line
	//  @pre operationLine is a line that has one of the four arithemtic operations (+, -,  *, /)
	//  @post operationLine will not be affected
	//  @param operationLine is line with a arithmetic operation 
	//  @return a string that stores the arithemetic operation in operationLine
	*/
	std::string whatOperator(std::string& operationLine);

	/**  Handle simple arithmetic operations +,-,*,/
	//  @pre operationstatement is a statement of the form -->  a = b (+,-,*,/) c    where a,b,c are operands 
	//  @post answervector will modified because assembly instructions for arithemtic statements will be included in it 
	//  @post VarValues will changed because the value of a in a = b (+,-,*,/) c wil be updated 
	//  @param answervector is a vector of string that already stores the assembly instructions from previous compiled c instructions 
	//  @param operationstatement is an arithmetic statement of the form a = b (+,-,*,/) c
	//  @param localVarsOffsets is the the container that stores all the local variables in the c file and their offsets 
	//  @param VarValues is the the container that stores all the local variables in the c file and their values 
	//  @param operB is a boolean that will tell us if operand b is an is an array in the form arrayName[something] where something is a variable then converion instruction(cltq) will be needed 
    //  @param operC is a boolean that will tell us if operand c is an is an array in the form arrayName[something] where something is a variable then converion instruction(cltq) will be needed 
	//  @return nothing but answervector will modified, VarValues will changed
	*/
	void HandleSimpleArithmetic(std::vector<std::string>& answervector, std::string& operationstatement, std::vector<std::pair<std::string, short>>& localVarsOffsets, std::vector<std::pair<std::string, int>>& VarValues, bool &operB, bool& operC);
  
	/** Generic For loop format is ----> for(int x=0;x<6;x++) 
	//  Handle 1st part of For Loop (int x=0) by adding loop variable counter in assembly code 
	//  @pre part will be the 1st part of the for loop and answervector will have the previous compiled c code in assembly
	//  @post answervector will modified because the assembly instructions to handle the 1st part of the for loop will be added to it like adding the counter variable x 
	//  @post VarValues will changed with the addition of a new variable and its value and localVarsOffsets will updated with a new variable and its offset
	//  @param part is the 1st part of the for loop (int x=0;) 
	//  @param answervector is a vector of string that already stores the assembly instructions from previous compiled c instructions
	//  @param localVarsOffsets is the container that stores all the local variables in the c file and their offsets 
	//  @param off is the offset of the new integer counter variable of the for loop 
	//  @param VarValues is the the container that stores all the local variables in the c file and their values 
	//  @return nothing but look at the post conditions written above 
	*/
	void handleFirstPart(std::string& part, std::vector<std::string>& answervector, std::vector<std::pair<std::string, short>>& localVarsOffsets, short&off, std::vector<std::pair<std::string, int>> &VarValues);

	/** Generic For loop format is ----> for(int x=0;x<6;x++) 
	//  Handle 2nd part of For Loop(x<6) and add first two lines of assembly code under the begining loop label that deal with comparsions and jumps  
	//  @pre part will be the second part of the for loop and answervector will have the previous compiled c code in assembly
	//  @pre the first label is already stored in answervector 
	//  @post answerVector will have the comparsion and condtional jump assembly instructions added to it like cmp, jl, jle, je, jg ..... 
	//  @param part is the second part of the for loop (x<6) 
	//  @param answervector is a vector of string that already stores the assembly instructions from previous compiled c instructions
	//  @param labelNum will be used to generate different labelNames for the for loop instructions 
	//  @param off is the offset of integer for loop counter variable x 
	//  @return nothing but look at the post condition written above
	*/
	void handleSecondPart(std::string& part, std::vector<std::string>& answervector, short &labelNum, short& off);


	/** Generic For loop format is ----> for(int x=0;x<6;x++)
	// 	Handle 3nd part of For Loop (x++) and adds the increment value assembly instructions that changes the value of the for loop counter variable
	//  @pre All the instructions in the for loop body have been compiled and added to answervector 
	//  @post answervector will have the assembly instructions that increment or decrement the value of the for loop counter variable
	//  @param part is the third part of the for loop (x++)
	//  @param answervector is a vector of string that already stores the assembly instructions from previous compiled c instructions
	//  @param off is the offset of the integer for loop counter variable x
	//  @return nothing but look at the post condition 
	*/
	void handleThirdPart(std::string& part, std::vector<std::string>& answervector,short& off);

	/** Function that handles the return statement it updates the answer vector by looking for the offset of the returned value 
	//  @pre returnStatement is actually an return statment and nothing else 
	//  @post answer will have the assembly instructions that handle the return statements of the c file 
	//  @param answer is a vector of string that already stores the assembly instructions from previous compiled c instructions
	//  @param returnStatement is the return statement itself 
	//  @param localVarsOffsets is the container that stores all the local variables in the c file and their offsets 
	//  @return nothing but look at the post condition
	*/
	void handleReturnStatement(std::vector<std::string>& answer, std::string &returnStatement, std::vector<std::pair<std::string, short>> & localVarsOffsets);
}

