typedef long long int Elements;

#include "Commands_Registers_Errors.h"
#include "Stack.h"
#include "Stack_Functions.cpp"

#define MAX_REGISTERS_NUMBER 10
#define MAX_CODE_SIZE 1000

const char* NAME_FILE_NATIVE_CODE_BY_DEFAULT = "Code.txt";
const char* READING = "r";
const size_t START_CAPACITY = 8;
const int ANSWER_POISON = -1000;
const char* REGISTERS[MAX_REGISTERS_NUMBER] = {"AX", "BX", "CX", "DX", "SI",
                                               "DI", "BP", "SP", "R8", "R9"};

struct Processor
{
  Elements* Code_Array;
  size_t Code_Size;
  size_t IP;
  Elements* Register_Array;
  struct Stack_t Stk;
  struct Stack_t Ret_Addr;
};

FILE* Fopen_File(const char* File_Name, const char* Mode);
Errors Fclose_File(FILE* File, const char* File_Name);
struct Processor* Proc_Init(void);
void Proc_Destroy(struct Processor* Proc);
Errors Read_File_Formating_Code(FILE* Native_File, Processor* Proc);
size_t Cnt_Num_Arg_For_Command(Elements Code_Array[], size_t i);
int Doing_Commands_In_Code(Processor* Proc);
int Hlt_C(bool Flag);
Errors Push_C(Processor* Proc);
Errors Pushr_C(Processor* Proc);
Errors Pop_C(Processor* Proc);
Errors Popr_C(Processor* Proc);
Errors Add_C(Processor* Proc);
Errors Sub_C(Processor* Proc);
Errors Mul_C(Processor* Proc);
Errors Div_C(Processor* Proc);
int Jmp_C(Processor* Proc);
int Jmp_Conditional_C(Processor* Proc, Elements Command);
Errors In_C(Processor* Proc);
Errors Out_C(Processor* Proc);
int Call_C(Processor* Proc);
int Ret_C(Processor* Proc);
void Dumb_C(Processor* Proc);
Errors Sqrt_C(Processor* Proc);
Errors Squ_C(Processor* Proc);
void Code_Dumb(Processor* Proc);
void Registers_Dumb(Processor* Proc);