#include "Commands_And_Registers.h"
#include "Stack.h"
#include "Functions.cpp"

#define MAX_REGISTERS_NUMBER 10
#define MAX_SYSTEM_REGISTERS_NUMBER 5
#define MAX_CODE_SIZE 1000

const char* NAME_FILE_NATIVE_CODE_BY_DEFAULT = "Code.txt";
const char* READING = "r";
const size_t START_CAPACITY = 8;
const int ANSWER_POISON = -1000;
const char* REGISTERS[MAX_REGISTERS_NUMBER] = {"AX", "BX", "CX", "DX", "SI",
                                               "DI", "BP", "SP", "R8", "R9"};
const char* SYSTEM_REGISTERS[MAX_SYSTEM_REGISTERS_NUMBER] = {"R10", "R11", "R12", "R13", "R14"};

struct Processor
{
  int* Code_Array;
  size_t Code_Size;
  size_t IP;
  int* Register_Array;
  int* System_Register_Array;
  size_t Free_System_Register;
  struct Stack_t Stk;
};

FILE* Fopen_File(const char* File_Name, const char* Mode);
int Fclose_File(FILE* File, const char* File_Name);
int Read_File_And_Formating_Code_Array(FILE* Native_File, Processor* Proc);
size_t Count_Number_Arg_For_Command(int Code_Array[], size_t i);
int Pass_Code_Array_And_Execution_Commands(Processor* Proc);
int HLT_C(bool Flag);
int PUSH_C(Processor* Proc);
int PUSHr_C(Processor* Proc);
int POP_C(Processor* Proc);
int POPr_C(Processor* Proc);
int ADD_C(Processor* Proc);
int SUB_C(Processor* Proc);
int MUL_C(Processor* Proc);
int DIV_C(Processor* Proc);
int JMP_C(Processor* Proc);
int JA_C(Processor* Proc);
int JAE_C(Processor* Proc);
int JB_C(Processor* Proc);
int JBE_C(Processor* Proc);
int JE_C(Processor* Proc);
int JNE_C(Processor* Proc);
int IN_C(Processor* Proc);
int OUT_C(Processor* Proc);
int CALL_C(Processor* Proc);
int RET_C(Processor* Proc);
void DUMB_C(Processor* Proc);
void Code_Dumb(Processor* Proc);
void Registers_Dumb(Processor* Proc);