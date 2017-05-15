/* machine.cpp - A software emulated hypothetical computer in 'c++'
 * This project is part of a teaching/learning experience to implement a
 * fairly simple 32 bit computer architecture in the 'c' language.  
 * Procedure oriented and in-line code rather than class oriented code 
 * is used quite a bit for speed.*/
 
 /* change history
 4/16/17 - change the stack to build downward
 4/16/17 - add 'test' command to run test code in cpu
 4/16/17 - adopt int32_t for machine registers, instructions and memory
 4/19/17 - shuffle classes, registers now private in CPU
 4/28/17 - improve UI allow multiple args on command line
 4/28/17 - stub out the first level instruction decode
 
 */
 
 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
/* Global constants */
#define NUM_REGISTERS 16
#define PCR_REGISTER 15  
#define SP_REGISTER 14
#define MEMORY_SIZE 0X10000 // Set to 32k for now,can be expanded to 20 bits

#define INSTRUCTION_INVALID 1 // return code for invalid instruction
#define INSTRUCTION_NOT_IMPLEMENTED 2  // defined but not implemented yet
#define INSTRUCTION_HALT 3 // return code for halt instruction encountered

/*  Global Variables  - For speed */

int32_t Memory[MEMORY_SIZE];


 

// Prototype class definitions
class CPU;
class Console;

//********************************************************************
// Class to implement the CPU
//********************************************************************
class CPU
{

	public:
		CPU();	// Constructor
		int Step(); // Step the CPU single instruction step
		int Run(); // Run the CPU
		int Test() ; // Run whatever code is in the test section
		int32_t Get_register_value (int register_number) ; // get value
		int Store_value_in_register(int register_number, int32_t value) ; // store value


	private:

		int32_t Regs[NUM_REGISTERS];
		int Execute  (int32_t instruction); // Execute an instruction
		int ProcessX7(int32_t instruction); // Process X7 non-zero instructions
		int ProcessX6(int32_t instruction); // Process X6 non-zero instructions
		int ProcessX5(int32_t instruction); // Process X5 non-zero instructions
		int ProcessX4(int32_t instruction); // Process X4 non-zero instructions
		int ProcessX3(int32_t instruction); // Process X3 non-zero instructions
		int ProcessX2(int32_t instruction); // Process X2 non-zero instructions
		int ProcessX1(int32_t instruction); // Process X1 non-zero instructions
};	

// CPU constructor  - set up object and clear memory and registers
CPU::CPU(void) {
	printf(" CPU object created \n");
	for (int i = 0; i<NUM_REGISTERS; i++){	//Clear the registers
		Regs[i] = 0;
	};
	for (int i = 0; i<MEMORY_SIZE; i++){	//Clear the memory
		Memory[i] = 0;
	};	
};
// CPU method to obtain register value (no value checking, do externally)
int32_t CPU::Get_register_value(int register_number) {

		return (Regs[ register_number ]);

}

// CPU method to store a value in a register (no value checking, do externally)
int CPU::Store_value_in_register (int register_number, int32_t value) {
	Regs [register_number] = value ;
	return 0 ;
}

// CPU method to handle instruction single step
int CPU::Step(void) {

	int address = Regs[PCR_REGISTER];
	int instruction = Memory[address];  // Instruction to be executed

	printf("CONS> Step -instruction at %08X is %08X \n",address,instruction);
	return Execute(instruction); // just return with execution code
}

// CPU method to execute an instruction
int CPU::Execute(int32_t instruction){
	
// look for all zero's, halt instruction
	if ( instruction == 0) { // halt
		return INSTRUCTION_HALT ;
	}
	
// look for non-zero X7 digit (high order hex digit)
	 else if ( (instruction & 0xF0000000) != 0 ){ // process non_zero X7
		return ProcessX7(instruction);
	}
// look for non-zero X6 digit 
	else if ( (instruction & 0x0F000000) != 0 ){ // process non_zero X6
		return ProcessX6(instruction);	
	}
// look for non-zero X5 digit
	else if ( (instruction & 0x00F00000) != 0) { // process non_zero X5
		return ProcessX5(instruction);
	}	
// look for non-zero X4 digit
	else if ( (instruction & 0x000F0000) != 0) { // process non-zero X4
		return ProcessX4(instruction);
	}
	
// look for non-zero X3 digit
	else if ( (instruction & 0x0000F000) != 0) { // process non-zero X3
		return ProcessX3(instruction);
	}
	
// look for non-zero X2 digit
	else if ( (instruction & 0x00000F00) != 0) { // process non-zero X2
		return ProcessX2(instruction);
	}	
	
// look for non-zero X1 digit
	else if ( (instruction & 0x000000F0) != 0) { // process non-zero X1
		return ProcessX1(instruction);
	}		
// no match, return bad instruction
	
	else return INSTRUCTION_NOT_IMPLEMENTED;



}
// CPU method to handle X7 != 0 (Memory reference instructions)
int CPU::ProcessX7(int32_t instruction) {
	
	printf ("Instruction decoded as X7 %08X instruction \n");

	int code = (instruction >> 28) & 0x0000000F ;// get op code
	printf("Hex code X7 is %01X \n",code);
	
	int idx_reg = (instruction >> 24) & 0x0000000F ; // get index reg
	printf("Index register is %01X \n",idx_reg);
	
	int dest_reg = (instruction >> 20) & 0x0000000F ; // get destination
	printf("Destination register is %01X \n",dest_reg);
	
	int32_t address = instruction & 0x000FFFFF ;// base address from instruction
	printf("Base address is %08X \n",address);
	
	if (idx_reg != 0) { // compute effective address
		address = address +  Regs[idx_reg] ;
		printf("After index, address is %08X \n",address);
	}
	
	// decode the instruction using a switch statement
	switch (code) {
		case 1: {  // load register
			Regs [dest_reg] = Memory [address] ;
			break;
		}
		case 2: { // store register
			Memory [address] = Regs [dest_reg] ;
			break;
		}
		case 3: { // add to register
			//!!!!!!!!!!!!!!!! needs overflow check !!!!!!!!!!!!!!!!
			Regs [dest_reg] += Memory [address] ;
			break;
		}
		case 4: { // subract from register
			//!!!!!!!!!!!!!!!! needs overflow check !!!!!!!!!!!!!!!!!
			Regs [dest_reg] -= Memory [address] ;
			break;
		}
		case 5: { // jump to address
			Regs [PCR_REGISTER] = address;
			return 0; // return OK to bypass PCR increment
			break;
		
		}
		case 6: { // call
			Regs [SP_REGISTER]-- ; // decrement the stack pointer
			Memory [ Regs [SP_REGISTER] ] = Regs [PCR_REGISTER]; // store return
			Regs [PCR_REGISTER] = address ; // transfer to address
			return 0; // return OK to bypasss PCR increment
		}
		default: { // instruction not implemented
			return INSTRUCTION_INVALID ;
		}

	}
	Regs [PCR_REGISTER]++ ; // increment the program counter			
	return 0;
}

// CPU method to handle X6 != 0 (Immediate instructions)
int CPU::ProcessX6(int32_t instruction) {


	printf ("Instruction decoded as X6 %08X instruction \n",instruction);

	int code = (instruction >> 24) & 0x0000000F ;// get op code
	printf("Hex code X6 is %01X \n",code);

	int dest_reg = (instruction >> 20) & 0x0000000F ; // get destination
	printf("Destination register is 08X \n",dest_reg);
	
	int32_t value = instruction & 0x000FFFFF ;// immediate value from instruction

	int32_t signed_value = value ; // first assume non negative
	if ( (value & 0x0008000) != 0) { // short number is negative
		signed_value = signed_value | 0xFFF ; // extend sign
	}
	printf("Value is %08X \n",value);
	printf("Value with sign is %08X \n",signed_value) ;
	
	
	// decode the instruction using a switch statement
	switch (code) {
		case 1: {  // load register immediate
			Regs [dest_reg] = value ;
			break;
		}
		case 2: { // load register signed
			Regs [dest_reg] = signed_value ;
			break;
		}
		case 3: { // add  immediate to register
			//!!!!!!!!!!!!!!!! needs overflow check !!!!!!!!!!!!!!!!
			Regs [dest_reg] += signed_value ;
			break;
		}
		case 4: { // subract immediate from register
			//!!!!!!!!!!!!!!!! needs overflow check !!!!!!!!!!!!!!!!!
			Regs [dest_reg] -= signed_value ;
			break;
		}
		case 5: { // OR immediate
			Regs [dest_reg] = Regs [dest_reg] | value;
			return 0; // return OK to bypass PCR increment
			break;
		
		}
		case 6: { // AND immediate
			Regs [dest_reg] = Regs [dest_reg] & value ;
			break;
		}
		case 7: { // XOR immediate
			Regs [dest_reg] = Regs [dest_reg] ^ value;	
			break;
		}		
		default: { // instruction not implemented
			return INSTRUCTION_INVALID ;
		}

	}
	Regs [PCR_REGISTER]++ ; // increment the program counter			
	return 0;
}

// CPU method to handle X5 != 0 (Shift instructions)
int CPU::ProcessX5(int32_t instruction) {


	printf ("Instruction decoded as X5 type instruction is %08X \n",instruction);

	int code = (instruction >> 20) & 0x0000000F ;// get op code
	printf("Hex code X5 is %01X \n",code);

	int dest_reg = (instruction >> 16) & 0x0000000F ; // get destination
	printf("Destination register is 08X \n",dest_reg);
	
	int shift_count = instruction & 0x0000001F ;// shift count
	printf("Shift count is %08X \n",shift_count);

	
	// decode the instruction using a switch statement
	switch (code) { 
		case 1: {  // Shift left logical
			Regs[dest_reg] = Regs[dest_reg] << shift_count ;
			break;
		}
		case 2: { // Shift right logical
			Regs[dest_reg] = Regs[dest_reg] >> shift_count ;			
			break;
		}
		case 3: { // Shift left arithmetic

			int cnt = 0 ; // counts how many we have done
			int32_t initial_sign = Regs[dest_reg] & 0x80000000 ;
			
			while (cnt < shift_count) {

				int sign = Regs[dest_reg] & 0x80000000 ;// isolate sign
				Regs[dest_reg] = Regs[dest_reg] << 1 ; // do the shift
				//!!!!!!!!!!!!!!!! needs overflow check !!!!!!!!!!!!!!!!
			
				cnt++; // increment counter ;
			}
			break;
		}
		
		case 4: { // Shift right arithmetic
			int cnt = 0 ; // counts how many we have done
			
			while (cnt < shift_count) {

				int sign = Regs[dest_reg] & 0x80000000 ;// isolate sign
				Regs[dest_reg] = Regs[dest_reg] >>  1 ; // do the shift
				if (sign == 0x80000000) { // if number was negative
					Regs[dest_reg] = Regs[dest_reg] | 0x80000000; //extend sign
				}
				cnt++; // increment counter ;
			}			

			break;
		}
		case 5: { // Shift left circular
			int cnt = 0 ; // counts how many we have done
			while (cnt < shift_count) {

				int sign = Regs[dest_reg] & 0x80000000 ;// isolate sign
				Regs[dest_reg] = Regs[dest_reg] << 1 ; // do the shift
				if (sign == 0x80000000) { // set low bit if sign set
					Regs[dest_reg] = Regs[dest_reg] | 0x000000001 ;
				}
				cnt++; // increment counter ;
			}	
				
					
			break;
		
		}
		case 6: { // Shift right circular
			int cnt = 0 ; // counts how many we have done
			while (cnt < shift_count) {

				int lsb = Regs[dest_reg] & 0x00000001 ;// isolate lsb
				Regs[dest_reg] = Regs[dest_reg] >> 1 ; // do the shift
				if (lsb == 0x00000001) { // set sign bit if lsb set
					Regs[dest_reg] = Regs[dest_reg] | 0x80000000 ;
				}
				
				cnt++; // increment counter ;
			}	
			break;
		}
	
		default: { // instruction not implemented
			return INSTRUCTION_INVALID ;
		}

	}
	Regs [PCR_REGISTER]++ ; // increment the program counter			
	return 0;
}

// CPU method to handle X4 != 0 (Register to register instructions)
int CPU::ProcessX4(int32_t instruction) {

	printf ("Instruction decoded as X4 type instruction is %08X \n",instruction);

	int code = (instruction >> 16) & 0x0000000F ;// get op code
	printf("Hex code X4 is %01X \n",code);

	int dest_reg = (instruction >> 8) & 0x0000000F ; // get destination
	printf("Destination register is 08X \n",dest_reg);
	
	int src_reg = instruction  & 0x0000000F ; // get source register
	printf("Destination register is 08X \n",dest_reg);	
	
	// decode the instruction using a switch statement
	switch (code) { 

		case 1: {  // Copy register
			Regs[dest_reg] = Regs[src_reg] ; // do the copy
			break ;
		}
		
		case 2: { // Add register
			Regs[dest_reg] += Regs[src_reg] ; // do the add
			//!!!!!!!!!!!!!!!!! need overflow check !!!!!!!!!!!!
			break; 
		}
		
		case 3: { // Subtract register
			Regs[dest_reg] -= Regs[src_reg] ; // do the subtract
			//!!!!!!!!!!!!!!!!! need overflow check !!!!!!!!!!!!
			break;
		}
		
		case 4: { // 'OR' registers
			Regs[dest_reg] = Regs[src_reg] | Regs[dest_reg] ;
			break;
		}
		
		case 5: { // 'AND' registers
			Regs[dest_reg] = Regs[src_reg] & Regs[dest_reg] ;
			break;			
		}
		
		case 6: { // 'XOR' registers
			Regs[dest_reg] = Regs[src_reg] ^ Regs[dest_reg] ;
			break;
		}
		
		case 7: { // skip greater
			if (Regs[src_reg] > Regs[dest_reg]) {
				Regs[PCR_REGISTER]++ ;
			}
			break;
		}
		
		case 8: { // skip greater or equal
			if (Regs[src_reg] >= Regs[dest_reg]) {
				Regs[PCR_REGISTER]++ ;
			}			
			break;
		}	
		
		case 9: { // skip equal
			if (Regs[src_reg] == Regs[dest_reg]) {
				Regs[PCR_REGISTER]++ ;
			}
			break;
		}
		
		case 0xA: { //  skip less than or equal
			if (Regs[src_reg] <= Regs[dest_reg]) {
				Regs[PCR_REGISTER]++ ;
			}
			break;
		}
		
		case 0xB: { // skip less than
			if (Regs[src_reg] < Regs[dest_reg]) {
				Regs[PCR_REGISTER]++ ;
			}
			break;
		}
		
		case 0xC: { // skip on overflow
//!!!!!!!!!!!!!!!!!!!!!11 need code !!!!!!!!!!!!!!!!!!!!!!			
			break;
		}
		

		case 0xD: { // skip no overflow
//!!!!!!!!!!!!!!!!!!!!!! need code !!!!!!!!!!!!!!!!!!!!!!			
			break;
		}
		
		default: { // instruction not implemented
			return INSTRUCTION_INVALID ;
		}

	}
			
	Regs [PCR_REGISTER]++ ; // increment the program counter			
	return 0;
}	

// CPU method to handle X3 != 0 (Single Register Instructions)
int CPU::ProcessX3(int32_t instruction) {
// !!!!!!!!!!!!!!! stub - need code !!!!!!!!!!!!!!!!!!!!!!!!!	
	Regs [PCR_REGISTER]++ ; // increment the program counter			
	return 0;
}
	
// CPU method to handle X2 != 0 (I/O instructions)
int CPU::ProcessX2(int32_t instruction) {	
	
	
//	printf ("Instruction decoded as X2 type instruction is %08X \n",instruction);

	int code = (instruction >> 8) & 0x0000000F ;// get op code
//	printf("Hex code X2 is %01X \n",code);
	
	int ioreg = instruction & 0x0000000F ; // get register
//  printf("IO register is %08X \n",ioreg);
	
	// decode the instruction using a switch statement
	switch (code) { 

		case 1: {  // Write character
			int c = Regs[ioreg] & 0xFF ; // isolate low byte
			putchar(c); // output the character
			break ;
		}	
		
		case 2: { // Read character
			int c ;
			c = getchar() ; // get a character
			Regs[ioreg] = (Regs[ioreg] & 0xFFFFFF00) | (c & 0xFF) ;
			break ;			
		}
		
		case 3: { // Write register contents line
			printf("Reg %1x = %08x \n",ioreg,Regs[ioreg]) ;
			break ;
		}
		
		default: { // instruction not implemented
			return INSTRUCTION_INVALID ;
		}

	} // end of switch decode
			
	Regs[PCR_REGISTER]++ ; // increment the program counter			
	return 0;
}			

// CPU method to handle X1 != 0 (Misc instructions)
int CPU::ProcessX1(int32_t instruction) {	


	printf ("Instruction decoded as X1 type instruction is %08X \n",instruction);

	int code = (instruction >> 4) & 0x0000000F ;// get op code
	printf("Hex code X1 is %01X \n",code);

	// decode the instruction using a switch statement
	switch (code) { 

		case 1: {  // No op

			break ;
		}	
		
		case 2: { // call return

			Regs[PCR_REGISTER] = Regs[SP_REGISTER] ; // go back via stack
			Regs[SP_REGISTER]++ ; // bump the stack
			return 0 ; // bypass PCR increment
		}
	
		default: {
			return INSTRUCTION_INVALID ;
		}
	} // end of decode switch

	Regs [PCR_REGISTER]++ ; // increment the program counter			
	return 0;
}	
// CPU method for tests, called by console with 'test' command

int CPU::Test(void) { // test code goes here, called by 'test' from console
	printf("Test routine entered \n") ;
}



// ******************************************************************
// Console support routines
// ******************************************************************  
// getarg function parses buffer into arguments, returning number
// of arguments.
#define MAX_ARGS 10 
#define MAX_ARG_SIZE 80 
int getarg(char *buffer, char argv[][MAX_ARG_SIZE]) {

	int arg_count = 0 ;
	char* ptr_string ;

	ptr_string = strtok(buffer, " \n");
	while (ptr_string != NULL) {


//		printf(" %d %s\n",arg_count,ptr_string);
		strcpy(&argv[arg_count][0],ptr_string) ;
		arg_count++ ;
		ptr_string = strtok (NULL," \t\n");
	}
	return arg_count ;
}
/************************************************************
 * Console Class
 * **********************************************************/
class Console
{
	public:
		Console() ; // Console constructor
		int Start();		// Console runs until terminated

	private:
		CPU cpu ; // CPU object
		void Print_a_register(int regnum);
		void Print_all_registers(void);
		int Get_register_number();
		void Print_memory_location(int address);
		
}; // end of Console class definition

Console::Console() {   //Console constructor
}

// Mainline console method to run the console
int Console::Start()
{
	char inbuf[81] ; // input buffer
	char argv [MAX_ARGS][MAX_ARG_SIZE] ; //parsed command
	int num_args ;  // number of arguments on command including command
	bool finish = false;
	do {
		printf("CONS?> ");
		fgets( inbuf,81,stdin);
		num_args = getarg(inbuf, argv) ; // parse line

// empty buffer		
		if (num_args == 0 ) {  // buffer was empty, do nothing
		} 
		
// "q" quit command
		else if (strcmp(argv[0],"q") == 0 ) { // quit command
			finish = true;
			break;
		}

// "xr" examine register command
		else if (strcmp(argv[0],"xr") == 0){  //examine register command

			if (num_args > 1 ) { // register number was on command line
				int reg_number ; 
				sscanf(argv[1],"%x",&reg_number);
				Print_a_register(reg_number);

			}

			else {  // none on command line, get it from  user			
				Print_a_register(Get_register_number());
				
			}

		}

// "xra" examine all registers command
		else if (strcmp(argv[0],"xra") == 0){ // examine all registers
			Print_all_registers();
		}	

// "help"  help command
		else if (strcmp(argv[0],"help") == 0){ // print help list
			printf("CONS>List of all commands \n");
			printf("help - prints this list \n");
			printf("xr - examine register, then prompts for register number in hex \n");
			printf("xra - examine all registersxra - prints all register \n");
			printf("dr - deposit in register, prompt for register and contents \n");
			printf("xm - examine memory, prompt location and number of locs in hex \n");
			printf("dm - deposit memory,prompt location terminate input with cntrl  \n");
			printf("s - step a single instruction \n");
			printf("test - run the test routine \n");
		}

// "dr"  deposit in register command
		else if (strcmp(argv[0],"dr") == 0){ // deposit a value in a register

			int reg_number ;
			int32_t value ; // value to enter in register
			
			if (num_args > 1 ) { // register number was on command line
				sscanf(argv[1],"%x",&reg_number);
			}
			else { // register not on command line, get it from user
				reg_number = Get_register_number() ;
			}

			if (num_args > 2) { // value was also on command line
				sscanf(argv[2],"%x",&value); // decode value
				
			}
			else { // value not on command line, get it from user
				
				printf("CONS?>enter value to deposit in hex ");
				fgets( inbuf,81,stdin);
				sscanf(inbuf,"%x",&value);
			}
			cpu.Store_value_in_register( reg_number, value );  // store the value 
		}

// "xm" examine memory command
		else if (strcmp(argv[0],"xm") == 0){  // examine memory
			int address;
			int number_words;
			
			if (num_args > 1 ) { // memory address was on command line
				sscanf(argv[1],"%x",&address);
			}			
			
			else { // address not on command line, get it 
				printf("CONS?>enter in hex memory location> ");
				fgets(inbuf,81,stdin);
				sscanf(inbuf,"%x ",&address) ;
				printf("\nCONS?>enter number of words in hex> ") ;
				fgets(inbuf,81,stdin);
				sscanf(inbuf,"%x ",&number_words) ;
				
			}

			if (num_args == 2 ) { // address but no word count was entered
				printf("CONS?>enter number of words in hex> ") ;
				fgets(inbuf,81,stdin);
				sscanf(inbuf,"%x ",&number_words) ;	
			}
						
			if (num_args > 2) { // word count was also on the initial command
				sscanf(argv[2],"%x",&number_words);
			}
				

			for (int i = 0; i < number_words; i++){ // do the dump
				Print_memory_location(address);
				address++;
			}
		}

// "dm" deposit memory command
		else if (strcmp(argv[0],"dm") == 0) { // deposit memory
			int address;
			int value;
			char inbuf[21];
			char* pointer_string;
			
			if (num_args > 1 ) { // memory address was on command line
				sscanf(argv[1],"%x",&address);
			}			
			
			else { // address not on command line, get it 
					printf("CONS> enter beginning memory address> ") ;
					fgets(inbuf,21,stdin);
					sscanf(inbuf,"%x ",&address) ;
			}
			
			if (num_args > 2) { // a single value was also supplied
				sscanf(argv[2],"%x",&value) ;
				Memory[address] = value;
			}
			else { // need a series of values
			
				printf("CONS> enter consecutive memory values, break with return only \n");
				printf("CONS %08X contents? > ",address);
				
				do {

					pointer_string=fgets( inbuf,21,stdin);
//					printf("String length is %d \n",strlen(pointer_string));
					if (strlen(pointer_string) >1){
						if (pointer_string != NULL)  {
							sscanf(pointer_string,"%x",&value);
							Memory[address] = value;
							address++;
							printf("CONS %08X contents? > ",address);
						}

					}	
				
				} while ( (pointer_string != NULL) && (strlen(pointer_string) > 1) );
				// deposit loop terminates on empty line of return only or cntrl/d 
			}	
		} // end of "dm" hander
		
// "s" single instruction step command
		else if (strcmp(argv[0],"s") == 0){ // step an instruction
			cpu.Step();
		}

// "test" execute test code command
		else if (strcmp(argv[0],"test") == 0) { //execute test routine
			cpu.Test();
		}

// 
		else {
			printf("Command not found \n");
		}

	} while (!finish);
	
	printf("Good day \n");
	return 0; // return success

};

// Console method to print a register
void Console::Print_a_register(int reg_number)
{
	if ( (reg_number >= 0) && (reg_number <NUM_REGISTERS)) {
		printf("CONS> Register %X = %08X \n",reg_number,
		  cpu.Get_register_value(reg_number));
	}
	else {
		printf("Illegal register number \n");
	}


	return;
};	
	
// Console method to print all registers
void Console::Print_all_registers(void)
{

	for ( int i= 0;i<NUM_REGISTERS;i++) {
		Print_a_register(i);
	}
	return;
};
	
// Console method to prompt for and get register number
int Console::Get_register_number(){
	int regnum ;
	char inbuf[81] ;
	while (true) {
		
		printf("CONS Register Number?> ");
		fgets(inbuf,81,stdin);
		sscanf(inbuf,"%x",&regnum);
		if ( (regnum >= 0) && (regnum <NUM_REGISTERS)) {
			return regnum;
		}
		else {
			printf("Illegal register number \n");
		}
	}
}

// Console method to print a memory location
void Console::Print_memory_location(int address){
	printf("CONS> %08X %08X \n",address,Memory[address]);
}				

// Mainline program - just turns control to the console until done 


int main(void)
{ 
	printf ("Hello world \n");
	Console cons ; // Create the console 
	return cons.Start();
	
	
}

