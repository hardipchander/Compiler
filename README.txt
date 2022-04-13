All the files for project are pch.h, pch.cpp, Action.h, Action.cpp, main.cpp, screenshot for test case 1,screenshot for test case 2, and the README
pch.h and pch.cpp are for the precompiled header 
Action.h and Action.cpp have a namespace that has all my Helper Functions that I need to convert the c instructions to assembly language 
main.cpp is the file where I read the Test Case source files line by line and display the assembly code   
Everything occurs in the main.cpp and the main.cpp file calls my HelperFunctions from Action.h and Action.cpp

The 8 rules the test case c source files must follow --------------------------------------------------------------------------------------------------------
(Note that the test case c source files in slides for the project description follow these 8 rules)

1) There are no spaces after the comma for the parameters --> like (int a,int b) is great but (int a, int b) is wrong and will not work 

2) There are no spaces in variable declartion after the name of the variable -->   int c=3; is good, but int c = 3; will not work

3) There are no spaces in between array element literals  --> int e[3]= {1,2,3,4}; is good but  int e[3]= {1,  2,  3, 4}; will not work 

4) There are no spaces in a variable arithemetic statement of the form c=a+b; 
  c=a+b; is great but c= b + a; will not work  

5) Every c instruction is indented to the left exmaple below 
int add(int a,int b) {
int c=0;
c=a+b;
return c;
}

6) There are no spaces in between the three for loop parts 
Example --> for(int x;x<6;x++) is good but for(int x; x<6; x++) will not work 

7) There are no spaces in the condition for the if statement 
Example -->  if(a<4) is good but if( a < 4 ) is not good

8) The if else structure is formatted like 
		if(condition) {
		    // body
		} else {
		    //body
		}

Notice Test Case 1 and Test Case 2 files from the Project Description slides follow these rules 
------------------------------------------------------------------------------------------------------------------------------------------------------------
Explanation of Code (My logic and approach)

As a global variable I have a vector of strings called output and output will get filled with assembly instructions as the project runs. 
After the program has finished reading the Test Case C files line by line, I will display every string element in output to terminal. 
Output will have all the assembly instructions after the Test Case C file is read. 
As a global variable I have a vector of pairs and each pair stores a variable name and its offset, this global variable is called localVars.
As a global variable I have a second vector of pairs and each pair stores a variable name and its value, this global variable is called varsNValues.

I have other global variables but they are simple and my comments in the code explains them 
Then inside the main function I use ifstream and the getline fucntion to read the Test Case C files line by line and each line is stored in a string variable named line.

Now the structure of my code follows the following format ------------------------------------------------------------------------------------------------------------

if(line is a increment value statment by itself like x++;){
	change the string line from x++; to x=x+1;
}
else if(line is a decrement value statment by itself like x--;) {
    change the string line from x--; to x=x-1;
}

if( if line is a function header enter) { // Process function header line 
	reset short global variable offset to -4 
	reset short global variable offsetForParameter7th to 16
	parse the line to get the function return type and place it in a global vector of strings  
	Then parse header line to get function name and place it in a global vector of strings 
	In output vector store functionName:,  pushq %rbp ,  movq %rsp, %rbp
	add "subq $32, %rsp" to output vector if main is not the only function in the c file 

	Then grab the first six parameters and move them at negative offsets of rbp and then store them in localVars with their offsets 
	For the parameters 7th or more store them in localVars with their offsets 
}

else if(if line is a function call in the main function of the c file ) { // Process Function Call in main()


}
-----------------------------------------------------------------------------------------------------------------------------------------------------------------------