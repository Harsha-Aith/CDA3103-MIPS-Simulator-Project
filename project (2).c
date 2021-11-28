/*
  Group Members: Amrit Murali, Sriharsha Aitharaju
  Course: CDA3103
  Due Date: 11/23/2021
  filename: project.c

  We have written many functions involved in the simulation of a  single cycle
  datapath for a mips processor in the C programming language.

  This program uses spimcore.c, spimcore.h, an input test file (.asc), to properly
  function.

  How to compile on command line (we used an Ubuntu-based bash environment):

    $ gcc -o spimcore spimcore.c project.c
    $ ./spimcore <inputFile>.asc

  Some of the commands that can be used include:...
    - m = MEMORY
    - s = STEP
    - c = CONTINUE
    - h = HALT
    - p = PRINT
    - g = CONTROLS
    - x = QUIT
*/

// include all libraries
#include <stdio.h>
#include <stdlib.h>
#include "spimcore.h"

/* ALU - checks the ALUControl, and determines the operation, executes this op,
         stores it within ALUresult, and gives Zero a value */
/* 10 Points */ // written by Harsha
void ALU(unsigned A,unsigned B,char ALUControl,unsigned *ALUresult,char *Zero)
{
  // temp result
  unsigned z;
  switch(ALUControl)
  {
    // First case, addition
    case 0:
      z = A + B;
      break;

    // Second case, subtraction
    case 1:
      z = A - B;
      break;

    // Third case, slt of integers A and B
    case 2:
      z = ((int) A < (int)B) ? 1 : 0;
      break;

    // Fourth case, slt of unsigned integers
    case 3:
      z = (A < B) ? 1 : 0;
      break;

    // Fifth case, And of unsigned A and B
    case 4:
      z = A & B;
      break;

    // Fifth case, And of unsigned A and B
    case 5:
      z = A | B;
      break;

    // Fifth case, And of unsigned A and B
    case 6:
      z = (B << 16);
      break;

    // Sixth case, NOT of A
    case 7:
      z = ~A;
      break;
  }
  // If the result of the ALU is 0, set z to 0. If not set z to 1.
  *Zero = (z == 0) ? 1 : 0;

  // Set the result of the ALU to the temp z
  *ALUresult = z;
}

/* instruction fetch - sees if PC is valid, and retrieves the instruction from mem */
/* 10 Points */ // written by Amrit
int instruction_fetch(unsigned PC,unsigned *Mem,unsigned *instruction)
{
  // check for halt (out of bounds or not word aligned)
  if (PC < 0x4000 && PC % 4 != 0) return 1;
  // check mem for instruction, (address is shifted two bits since actual address is PC/4)
  *instruction = Mem[PC >> 2];
  return 0;
}

/* instruction partition - splits the instruction into the different fields */
/* 10 Points */ // written by Harsha
void instruction_partition(unsigned instruction, unsigned *op, unsigned *r1,unsigned *r2, unsigned *r3, unsigned *funct, unsigned *offset, unsigned *jsec)
{
  // the hex number singles out the necessary bits for each field
  *op = (instruction & 0xfc000000) >> 26;
  *r1 = (instruction & 0x03e00000) >> 21;
  *r2 = (instruction & 0x001f0000) >> 16;
  *r3 = (instruction & 0x0000f800) >> 11;
  *funct = instruction & 0x0000003f;
  *offset = instruction & 0x0000ffff;
  *jsec = instruction & 0x03ffffff;
}

/* instruction decode - set control signals based on opcode */
/* 15 Points */ // written by Amrit
int instruction_decode(unsigned op,struct_controls *controls)
{
  // coverted binary number for op to the decimal equivalent
  if (op == 0) // is instruction R Type?
  {
    controls->RegDst = 1;
    controls->ALUSrc = 0;
    controls->MemtoReg = 0;
    controls->RegWrite = 1;
    controls->MemRead = 0;
    controls->MemWrite = 0;
    controls->Branch = 0;
    controls->Jump = 0;
    controls->ALUOp = 0b111;
  }
  else if (op == 35) // is instruction load?
  {
    controls->RegDst = 0;
    controls->ALUSrc = 1;
    controls->MemtoReg = 1;
    controls->RegWrite = 1;
    controls->MemRead = 1;
    controls->MemWrite = 0;
    controls->Branch = 0;
    controls->Jump = 0;
    controls->ALUOp = 0b000;
  }
  else if (op == 43) // is instruction store?
  {
    controls->RegDst = 2;
    controls->ALUSrc = 1;
    controls->MemtoReg = 2;
    controls->RegWrite = 0;
    controls->MemRead = 0;
    controls->MemWrite = 1;
    controls->Branch = 0;
    controls->Jump = 0;
    controls->ALUOp = 0b000;
  }
  else if (op == 4) // is instruction a branch?
  {
    controls->RegDst = 2;
    controls->ALUSrc = 0;
    controls->MemtoReg = 2;
    controls->RegWrite = 0;
    controls->MemRead = 0;
    controls->MemWrite = 0;
    controls->Branch = 1;
    controls->Jump = 0;
    controls->ALUOp = 0b001;
  }
  else if (op == 8) // is instruction an addi?
  {
    controls->RegDst = 0;
    controls->ALUSrc = 1;
    controls->MemtoReg = 0;
    controls->RegWrite = 1;
    controls->MemRead = 0;
    controls->MemWrite = 0;
    controls->Branch = 0;
    controls->Jump = 0;
    controls->ALUOp = 0b000;
  }
  else if (op == 15) // is instruction an load upper immediate?
  {
    controls->RegDst = 0;
    controls->ALUSrc = 1;
    controls->MemtoReg = 0;
    controls->RegWrite = 1;
    controls->MemRead = 0;
    controls->MemWrite = 0;
    controls->Branch = 0;
    controls->Jump = 0;
    controls->ALUOp = 0b110;
  }
  else if (op == 10) // is instruction set less than immediate?
  {
    controls->RegDst = 0;
    controls->ALUSrc = 1;
    controls->MemtoReg = 0;
    controls->RegWrite = 1;
    controls->MemRead = 0;
    controls->MemWrite = 0;
    controls->Branch = 0;
    controls->Jump = 0;
    controls->ALUOp = 0b010;
  }
  else if (op == 11) // is instruction set less than immediate unsigned?
  {
    controls->RegDst = 0;
    controls->ALUSrc = 1;
    controls->MemtoReg = 0;
    controls->RegWrite = 1;
    controls->MemRead = 0;
    controls->MemWrite = 0;
    controls->Branch = 0;
    controls->Jump = 0;
    controls->ALUOp = 0b011;
  }
  else if(op == 2) // is instruction jump?
  {
    controls->RegDst = 0;
    controls->ALUSrc = 0;
    controls->MemtoReg = 0;
    controls->RegWrite = 0;
    controls->MemRead = 0;
    controls->MemWrite = 0;
    controls->Branch = 0;
    controls->Jump = 1;
    controls->ALUOp = 0b000;
  }
  else return 1; // invalid opcode
  return 0;
}

/* Read Register - Set the values r1 and r2 in reg and write them to data1 and data2 */
/* 5 Points */ // written by Harsha
void read_register(unsigned r1,unsigned r2,unsigned *Reg,unsigned *data1,unsigned *data2)
{
    *data1 = Reg[r1];
    *data2 = Reg[r2];
}

/* Sign Extend - sign extends 16 bit num to 32 bit number */
/* 10 Points */ // written by Amrit
void sign_extend(unsigned offset,unsigned *extended_value)
{
  // offset is 16 bits, extended is 32 bits
  unsigned numBits = 16;

  // find the MSB for offset
  unsigned signBit = offset >> (numBits - 1);

  // if offset is positive, add 0s to the left of offset
  if(signBit == 0) *extended_value = (0x0000ffff) & offset;
  // if offset is negative, add 1s to the left of offset
  else *extended_value = 0xffff0000 | offset;
}

/* ALU operations - for R type, determine ALUOp based on function decode, and if
ALUSRC is 1 use the extended value, and then call the ALU function for all valid instructions */
/* 10 Points */ // written by Harsha
int ALU_operations(unsigned data1,unsigned data2,unsigned extended_value,unsigned funct,char ALUOp,char ALUSrc,unsigned *ALUresult,char *Zero)
{
  // ALUSRC (2nd input of ALU)
  // choose b/w reading data 2 and the sign ext. value
  if (ALUSrc == 1) data2 = extended_value;

  // if instruction is an r type, based on the function, choose a certain ALUresult
  if (ALUOp == 7) // 7 == 0b111
  {
    if (funct == 32) ALUOp = 0; // add
    else if (funct == 34) ALUOp = 1; // sub
    else if (funct == 36) ALUOp = 4 ;// and
    else if (funct == 37) ALUOp = 5; // or
    else if (funct == 42) ALUOp = 2; // slt
    else if (funct == 43) ALUOp = 3;// sltu
    else return 1; // r type funct is invalid -> halt
  }
  ALU(data1, data2, ALUOp, ALUresult, Zero);
  //ALU Op is found and no halt occurs
  return 0;
}

/* Read / Write Memory - reads and writes to memory after checking for valid address */
/* 10 Points */ // written by Amrit Murali
int rw_memory(unsigned ALUresult,unsigned data2,char MemWrite,char MemRead,unsigned *memdata,unsigned *Mem)
{
  // invalid address is a halt condition
  if ((MemWrite == 1 || MemRead == 1) && ((ALUresult % 4) != 0))
    return 1;
  // permission to read?, read memory into data
  if (MemRead == 1)
  {
    *memdata = Mem[ALUresult >> 2];
  }
  // permission to write?, write data to register
  else if (MemWrite == 1)
  {
    Mem[ALUresult >> 2] = data2;
  }
  // no halt condition, function ran smoothly
  return 0;
}

/* Write Register - it writes the data (ALUresult or memData) to the address of
the register(r2 or r3) */
/* 10 Points */ // written by Harsha
void write_register(unsigned r2,unsigned r3,unsigned memdata,unsigned ALUresult,char RegWrite,char RegDst,char MemtoReg,unsigned *Reg)
{
  // if writes to memory and memory is sent to reg file
  if(MemtoReg == 1 && RegWrite == 1 && RegDst == 0)
    Reg[r2] = memdata;
  // Represents add, subtract, and, or, slt
  if(MemtoReg == 0 && RegWrite == 1 && RegDst == 1)
    Reg[r3] = ALUresult;
  // i type instructions that write to register file
  if(MemtoReg == 0 && RegWrite == 1 && RegDst == 0)
    Reg[r2] = ALUresult;
}

/* PC update - updates program counter */
/* 10 Points */ // amrit
void PC_update(unsigned jsec,unsigned extended_value,char Branch,char Jump,char Zero,unsigned *PC)
{
  *PC = *PC + 4;
  if (Jump == 1) // jump to j sec and hold first 4 bits of PC
  {
    *PC = (jsec << 2) | (0xf0000000 & *PC);
  } else if (Branch == 1 && Zero == 1) // branching if zero is asserted
  {
    *PC = *PC + (extended_value << 2);
  }
}
