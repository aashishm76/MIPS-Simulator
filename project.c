// Aashish Madamanchi
// aa109958


#include "spimcore.h"
#include <stdio.h>


// ALU Function
void ALU(unsigned A,unsigned B,char ALUControl,unsigned *ALUresult,char *Zero)
{
  if ((int)ALUControl == 0)
  {
    // Z = A + B
    *ALUresult = A + B;
  }
  //  Z = A - B
  else if((int)ALUControl == 1)
  {
    *ALUresult = A - B;
  }
  // If A < B, Z = 1, otherwise Z = 0
  else if((int)ALUControl == 2)
  {
    if((signed) A < (signed) B)
      *ALUresult = 1;
    else
      *ALUresult = 0;
  }
  // If A < B, Z = 1, otherwise Z = 0 UNSIGNED
  else if((int)ALUControl == 3)
  {
    if(A < B)
      *ALUresult = 1;
    else
      *ALUresult = 0;
  }
  // Z = A AND B
  else if((int)ALUControl == 4)
  {
    *ALUresult = A & B;
  }
  // Z = A OR B
  else if((int)ALUControl == 5)
  {
    *ALUresult = A | B;
  }
  // Shift left B by 16 bits
  else if((int)ALUControl == 6)
  {
    *ALUresult = B << 16;
  }
  // Z = NOT A
  else if((int)ALUControl == 7)
  {
    *ALUresult = ~A;
  }

  //Now check to see if the result is zero, and set the zero bit
  if(*ALUresult == 0)
    *Zero = 1;
  else
    *Zero = 0;

}

// Instruction Fetch
int instruction_fetch(unsigned PC,unsigned *Mem,unsigned *instruction)
{
	//Multiply by 4 to get the proper address in memory
	unsigned index = PC >> 2;

  // Perform if byte address not set
	if(PC % 4 != 0)
	    return 1;

   // Set intruction equal to memory at proper address
	*instruction = Mem[index];
	return 0;
}

// Instruction Partition
void instruction_partition(unsigned instruction, unsigned *op, unsigned *r1,unsigned *r2, unsigned *r3, unsigned *funct, unsigned *offset, unsigned *jsec)
{

	*op = (instruction >> 26) & 0b00000000000000000000000000111111; // instruction[31-26]
	*r1 = (instruction >> 21) & 0b11111; // instruction[25-21]
	*r2 = (instruction >> 16) & 0b11111; // instruction[20-16]
	*r3 = (instruction >> 11) & 0b11111; // instruction[15-11]
	*funct = instruction & 0b00000000000000000000000000111111; // instruction[5-0]
	*offset = instruction & 0b00000000000000001111111111111111; // instruction[15-0]
	*jsec = instruction & 0b00000011111111111111111111111111; // instruction[25-0]
}

// Instruction Decode
int instruction_decode(unsigned op,struct_controls *controls)
{
  if(op == 0)
  {
    // R Type Instructions
    controls->RegDst = 1;
    controls->RegWrite = 1;
    controls->ALUSrc = 0;
    controls->MemRead = 0;
    controls->MemWrite = 0;
    controls->MemtoReg = 0;
    controls->Jump = 0;
    controls->Branch = 0;
    controls->ALUOp = 7;
  }
  // I Type Instructions
  else if (op == 8)
  {
    controls->RegDst = 0;
    controls->RegWrite = 1;
    controls->ALUSrc = 1;
    controls->MemRead = 0;
    controls->MemWrite = 0;
    controls->MemtoReg = 0;
    controls->Jump = 0;
    controls->Branch = 0;
    controls->ALUOp = 0;
  }
  else if(op == 10)
  {
    // slt
    controls->RegDst = 1;
    controls->RegWrite = 1;
    controls->ALUSrc = 0;
    controls->MemRead = 0;
    controls->MemWrite = 0;
    controls->MemtoReg = 0;
    controls->Jump = 0;
    controls->Branch = 0;
    controls->ALUOp = 2;
  }
  else if(op == 11)
  {
    // sltu
    controls->RegDst = 1;
    controls->RegWrite = 1;
    controls->ALUSrc = 0 ;
    controls->MemRead = 0;
    controls->MemWrite = 0;
    controls->MemtoReg = 0;
    controls->Jump = 0;
    controls->Branch = 0;
    controls->ALUOp = 3;
  }
  else if(op == 4)
  {
    // beq (branch on equal)
    controls->RegDst = 2;
    controls->RegWrite = 0;
    controls->ALUSrc = 0;
    controls->MemRead = 0;
    controls->MemWrite = 0;
    controls->MemtoReg = 2;
    controls->Jump = 0;
    controls->Branch = 1;
    controls->ALUOp = 1;
  }
  else if(op == 35)
  {
    // lw (load word)
    controls->RegDst = 0;
    controls->RegWrite = 1;
    controls->ALUSrc = 1;
    controls->MemRead = 1;
    controls->MemWrite = 0;
    controls->MemtoReg = 1;
    controls->Jump = 0;
    controls->Branch = 0;
    controls->ALUOp = 0;
  }
  else if(op == 15)
  {
    // lui
    controls->RegDst = 0;
    controls->RegWrite = 1;
    controls->ALUSrc = 1;
    controls->MemRead = 0;
    controls->MemWrite = 0;
    controls->MemtoReg = 0;
    controls->Jump = 0;
    controls->Branch = 0;
    controls->ALUOp = 6;
  }
  else if(op == 43)
  {
    //sw (store word)
    controls->RegDst = 2;
    controls->RegWrite = 0;
    controls->ALUSrc = 1;
    controls->MemRead = 0;
    controls->MemWrite = 1;
    controls->MemtoReg = 2;
    controls->Jump = 0;
    controls->Branch = 0;
    controls->ALUOp = 0;
  }

  // J Type Instructions
  // Jump
  else if(op == 2)
  {
    controls->RegDst = 0;
    controls->RegWrite = 0;
    controls->ALUSrc = 0;
    controls->MemRead = 0;
    controls->MemWrite = 0;
    controls->MemtoReg = 0;
    controls->Jump = 1;
    controls->Branch = 0;
    controls->ALUOp = 0;
  }
  else
  {
    return 1;
  }

  return 0;
}

// Read Register
void read_register(unsigned r1,unsigned r2,unsigned *Reg,unsigned *data1,unsigned *data2)
{
    // Populate data1 and data2 with the values from register array
    *data1 = Reg[r1];
    *data2 = Reg[r2];
}

// Sign Extend
void sign_extend(unsigned offset,unsigned *extended_value)
{
    unsigned negative = offset >> 15;

    // Extend with 1s
    if(negative == 1)
        *extended_value = offset | 0xFFFF0000;
    else
        *extended_value = offset & 0x0000FFFF;
}

// ALU Operations
int ALU_operations(unsigned data1,unsigned data2,unsigned extended_value,unsigned funct,char ALUOp,char ALUSrc,unsigned *ALUresult,char *Zero)
{
  // If the ALUSrc is 1, set data2 to extended_value
	if (ALUSrc == 1)
    data2 = extended_value;

	// 7 is R-type instruction
  if (ALUOp == 7)
  {
    // Use a switch statement too make it easy for comparisons
  	switch(funct)
    {
  	 	// add
  	 	case 32:
  	 		ALUOp = 0;
  	 		break;
  	 	// subtract
  	 	case 34:
  	 		ALUOp = 1;
  	 		break;
  	 	// set less than signed
  	 	case 42:
  	 		ALUOp = 2;
    		break;
    	// set less than unsigned
    	case 43:
    		ALUOp = 3;
    		break;
    	// and
    	case 36:
    		ALUOp = 4;
    		break;
    	// or
    	case 37:
    		ALUOp = 5;
    		break;
    	// shift left right variable
    	case 4:
    		ALUOp = 6;
    		break;
    	// nor
    	case 39:
    		ALUOp = 7;
    		break;
    	// default for halt or don't care
    	default:
    		return 1;
    }
    // call ALU function
    ALU(data1, data2, ALUOp, ALUresult, Zero);
    }
  else
  {
  	// call ALU for non-functions
  	ALU(data1, data2, ALUOp, ALUresult, Zero);
  }

  return 0;
}

// Read/Write Memory
int rw_memory(unsigned ALUresult,unsigned data2,char MemWrite,char MemRead,unsigned *memdata,unsigned *Mem)
{
  // Read data from ALUresult * 4 index in Memory.
  if(MemRead == 1)
    if((ALUresult % 4) == 0)
      *memdata = Mem[ALUresult >> 2];
    else
      return 1;

  // Write data to ALUresult * 4 index in Memory
  if(MemWrite == 1)
    if((ALUresult % 4) == 0)
      Mem[ALUresult >> 2] = data2;
    else
      return 1;


  return 0;
}

// Write Register
void write_register(unsigned r2,unsigned r3,unsigned memdata,unsigned ALUresult,char RegWrite,char RegDst,char MemtoReg,unsigned *Reg)
{
    if(RegWrite==1){
        // Memory to register
        if(MemtoReg == 1 && RegDst == 0)
            Reg[r2] = memdata;
        // Memory to register (to r3 specifically)
        else if(MemtoReg == 1 && RegDst == 1)
            Reg[r3] = memdata;

        // Store the result of this in r2
        else if(MemtoReg == 0 && RegDst == 0)
            Reg[r2] = ALUresult;
        // Store the result of this in r3
        else if(MemtoReg == 0 && RegDst == 1)
            Reg[r3] = ALUresult;
    }
}

// PC Update
void PC_update(unsigned jsec,unsigned extended_value,char Branch,char Jump,char Zero,unsigned *PC)
{
    // Always increment by 4
    *PC += 4;

    // If branching add the extended_value (bitshifted * 4))
    if(Zero == 1 && Branch == 1)
        *PC += extended_value << 2;

    // If jumping, jump to the Jump register, and multiply by 4
    if(Jump == 1)
        *PC = (jsec << 2) | (*PC & 0xf0000000);
}
