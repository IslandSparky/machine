/* machine.cpp - A software emulated hypothetical computer in 'c'
 * This project is part of a teaching/learning experience to implement a
 * fairly simple 32 bit computer architecture in the 'c' language.  
 *  Procedure oriented and in-line code rather than class oriented code 
 * is used quite a bit for speed.*/
 
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

/*  Global Variables  - For speed */
long int Regs[NUM_REGISTERS];
long int Memory[MEMORY_SIZE];

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
		int Execute(int instruction); // Execute an instruction
		int ProcessX7(int instruction); // Process X7 non-zero instructions
		int ProcessX6(int instruction); // Process X6 non-zero instructions
	private:

};
	
// CPU constructor  - set up object and clear memory and registers
CPU::CPU(void)
{
	printf(" CPU object created \n");
	for (int i = 0; i<NUM_REGISTERS; i++){	//Clear the registers
		Regs[i] = 0;
	};
	for (int i = 0; i<MEMORY_SIZE; i++){	//Clear the memory
		Memory[i] = 0;
	};	
};

// CPU method to handle instruction single step
int CPU::Step(void) {

	int address = Regs[PCR_REGISTER];
	int instruction = Memory[address];  // Instruction to be executed

	printf("CONS> Step intruction at %08X is %08X \n",address,instruction);
	return Execute(instruction); // just return with execution code
}

// CPU method to execute an instruction
int CPU::Execute(int instruction){
	
// look for non-zero X7 digit (high order hex digit)
	if ( (instruction || 0xF0000000) != 0 ){ // look for non_zero X7
		return ProcessX7(instruction);
	}
// look for non-zero X6 digit 
	if ( (instruction || 0x0F000000) != 0 ){ // look for non_zero X7
		return ProcessX6(instruction);	
	}
	// Rest of decode goes here *************************************
	return INSTRUCTION_NOT_IMPLEMENTED;
}
// CPU method to handle X7 != 0 (Memory reference instructions)
int CPU::ProcessX7(int instruction) {
	

	
	printf ("Instruction decoded as X7 %08X instruction \n");

	int code = (instruction >> 28) & 0x0000000F ;// get op code
	printf("Hex code X7 is %08X \n",code);
	
	int idx_reg = (instruction >> 24) & 0x0000000F ; // get index reg
	printf("Index register iis 08X \n",idx_reg);
	
	int dest_reg = (instruction >> 20) & 0x0000000F ; // get destination
	printf("Destination register is 08X \n",dest_reg);
	
	int address = instruction & 0x000FFFFF ;// base address from instruction
	printf("Base address is 08X \n",address);
	
	if (idx_reg != 0) { // compute effective address
		address = address +  Regs[idx_reg] ;
		printf("After index, address is 08X \n",address);
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
			Memory [ Regs [SP_REGISTER] ] = Regs [PCR_REGISTER]; // store return
			Regs [SP_REGISTER]++ ; // bump the stack pointer
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
int CPU::ProcessX6(int instruction) {
	

	
	printf ("Instruction decoded as X6 %08X instruction \n");

	int code = (instruction >> 24) & 0x0000000F ;// get op code
	printf("Hex code X6 is %08X \n",code);

	int dest_reg = (instruction >> 20) & 0x0000000F ; // get destination
	printf("Destination register is 08X \n",dest_reg);
	
	int value = instruction & 0x000FFFFF ;// immediate value from instruction

	int signed_value = value ; // first assume non negative
	if ( (value & 0x0008000) != 0) { // short number is negative
		signed_value = signed_value || 0xFFF ; // extend sign
	}
	printf("Value is 08X \n",value);
	printf("Value with sign is \n",signed_value) ;
	
	
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
			Regs [dest_reg] += value ;
			break;
		}
		case 4: { // subract immediate from register
			//!!!!!!!!!!!!!!!! needs overflow check !!!!!!!!!!!!!!!!!
			Regs [dest_reg] -= value ;
			break;
		}
		case 5: { // OR immediate
			Regs [dest_reg] = Regs [dest_reg] || value;
			return 0; // return OK to bypass PCR increment
			break;
		
		}
		case 6: { // AND immediate
			Regs [dest_reg] = Regs [dest_reg] && value ;
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



// ******************************************************************
//  Class to implement the console interaction
// ******************************************************************  
class Console
{
	public:
	
		int Start();		// Console runs until terminated

	private:
	
		void Print_a_register(int regnum);
		void Print_all_registers(void);
		int Get_register_number();
		void Print_memory_location(int address);
		
}; // end of Console class definition


// Mainline console method to run the console
int Console::Start()
{
	CPU cpu; //Create the cpu object
	char inbuf[81];
	char command[81];
	bool finish = false;
	do {
		printf("CONS?> ");
		scanf("%s",&command);

		if (strcmp(command,"q") == 0 ) { // quit command
			finish = true;
			break;
		}

		else if (strcmp(command,"xr") == 0){  //examine register command
			Print_a_register(Get_register_number());
		}

		else if (strcmp(command,"xra") == 0){ // examine all registers
			Print_all_registers();
		}	

		else if (strcmp(command,"help") == 0){ // print help list
			printf("CONS>List of all commands \n");
			printf("help - prints this list \n");
			printf("xr - examine register, then prompts for register number in hex \n");
			printf("xra - examine all registersxra - prints all register \n");
			printf("dr - deposit in register, prompt for register and contents \n");
			printf("xm - examine memory, prompt location and number of locs in hex \n");
			printf("dm - deposit memory,prompt location terminate input with cntrl  \n");
			printf("s - step a single instruction \n");
			
		}

		else if (strcmp(command,"dr") == 0){ // deposit a value in a register
			int regnum = Get_register_number();
			int value;
			printf("CONS?>enter value to deposit in hex ");
			scanf("%x",&value);
			Regs[regnum] = value;  // store the value in the register
		}

		else if (strcmp(command,"xm") == 0){  // examine memory
			int address;
			int number_words;
			printf("CONS?>enter in hex memory location and number of words ");
			scanf("%x%x",&address,&number_words);
			for (int i = 0; i < number_words; i++){
				Print_memory_location(address);
				address++;
			}
		}

		else if (strcmp(command,"dm") == 0) { // deposit memory
			int address;
			int value;
			char inbuf[21];
			char* pointer_string;
			printf("CONS?>enter the initial memory address to fill>");
			scanf("%x",&address);
			printf("CONS> enter consecutive memory values, break with  cntrl d (EOF) \n");
			printf("CONS %08X contents ?> ",address);
			do {

				pointer_string=fgets( inbuf,21,stdin);
				if (inbuf[0] != '\n') {
					if (pointer_string != NULL)  {
						sscanf(pointer_string,"%x",&value);
						Memory[address] = value;
						address++;
						printf("CONS %08X contents ?> ",address);
					}

				}	
				
			} while (pointer_string != NULL);
			printf("\n");
		} // deposit loop terminates on EOF ctrl/d				

		else if (strcmp(command,"s") == 0){ // step an instruction
			cpu.Step();
		}

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
	printf("CON> Register %X = %08X \n",reg_number,Regs[reg_number]);
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
	while (true) {
		
		printf("CONS Register Number?> ");
		scanf("%x",&regnum);
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
	printf("CONS> %08X  %08X \n",address,Memory[address]);
}				

// Mainline program - just turns control to the console until done 


int main(void)
{ 
	printf ("Hello world \n");
	Console cons ; // Create the console 
	cons.Start();
	return 0;
	
}

