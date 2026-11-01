#include "Virtual_Machine.h"

int main(int argc, char* argv[])
{
  if (argc < 2)
    printf("WARNING: too few command-line arguments\n");

  const char *Name_File_Native_Code = (argc >= 2) ? argv[1] : NAME_FILE_NATIVE_CODE_BY_DEFAULT;

  struct Processor* Proc = Proc_Init();
  if (Proc == NULL)
  {
    Proc_Destroy(Proc);
    printf("ERROR: allocate memory error\n");
    return ERROR_ALLOCATION;
  }

  FILE* Native_File = Fopen_File(Name_File_Native_Code, READING);
  if (Native_File == NULL)
  {
    Proc_Destroy(Proc);
    return ERROR_OPENING_FILE;
  }

  if (Read_File_Formating_Code(Native_File, Proc) != ERROR_NOT)
  {
    Fclose_File(Native_File, Name_File_Native_Code);
    Proc_Destroy(Proc);
    return ERROR_CLOSING_FILE;
  }

  Clear_Log_File();

  if (Doing_Commands_In_Code(Proc) == ANSWER_POISON)
  {
    Proc_Destroy(Proc);
    printf("ERROR: execution error\n");
    return ERROR_DOING;
  }

  if (Fclose_File(Native_File, Name_File_Native_Code) != ERROR_NOT)
  {
    Proc_Destroy(Proc);
    return ERROR_CLOSING_FILE;
  }

  Proc_Destroy(Proc);
  printf("Successful completion of the program\n");

  return ERROR_NOT;
}

FILE* Fopen_File(const char* File_Name, const char* Mode)
{
  FILE* File = fopen(File_Name, Mode);
  if (File == NULL)
  {
    printf("ERROR: can't open file %s\n", File_Name);
    return NULL;
  }

  return File;
}

Errors Fclose_File(FILE* File, const char* File_Name)
{
  if (File == NULL)
  {
    printf("ERROR: invalid pointer to file %s\n", File_Name);
    return ERROR_CLOSING_FILE;
  }

  if (fclose(File) == EOF)
    printf("WARNING: can't close file %s\n", File_Name);

  return ERROR_NOT;
}

struct Processor* Proc_Init(void)
{
  struct Processor* Proc = (struct Processor*)calloc(1, sizeof(struct Processor));
  if (Proc == NULL)
  {
    Proc_Destroy(Proc);
    printf("ERROR: can't allocate memory for Proc\n");
    return NULL;  
  }

  Proc->Code_Array = (Elements*)calloc(MAX_CODE_SIZE, sizeof(Elements));
  Proc->Register_Array = (Elements*)calloc(MAX_REGISTERS_NUMBER, sizeof(Elements));
  if (Proc->Code_Array == NULL || Proc->Register_Array == NULL)
  {
    Proc_Destroy(Proc);
    printf("ERROR: can't allocate memory for Code or Registers\n");
    return NULL;
  }

  Stack_Error_Status Err = Stack_Init(&(Proc->Stk), START_CAPACITY
                                      ON_DBG(, "Stk", __FILE__, __LINE__));
  if (Err < 0)
  {
    Proc_Destroy(Proc);
    printf("ERROR: memory allocation error\n");
    return NULL;
  }

  Err = Stack_Init(&(Proc->Ret_Addr), START_CAPACITY ON_DBG(, "Ret_Addr", __FILE__, __LINE__));
  if (Err < 0)
  {
    Proc_Destroy(Proc);
    printf("ERROR: memory allocation error\n");
    return NULL;
  }

  return Proc;
}

void Proc_Destroy(struct Processor* Proc)
{
  if (Proc == NULL)
    return;
  if (&(Proc->Stk) != NULL)
    Stack_Destroy(&Proc->Stk);
  if (&(Proc->Ret_Addr) != NULL)
    Stack_Destroy(&Proc->Ret_Addr);

  free(Proc->Code_Array);
  free(Proc->Register_Array);
  free(Proc);

  return;
}

Errors Read_File_Formating_Code(FILE* Native_File, Processor* Proc)
{
  Elements Command_Value = 0;
  while (fscanf(Native_File, "%lld", &Command_Value) > 0 && Proc->IP < MAX_CODE_SIZE)
  {
    Proc->Code_Array[Proc->IP] = Command_Value;
    (Proc->IP)++;
  }

  Proc->Code_Size = Proc->IP;
  return ERROR_NOT;
}

/*{
  Elements Command_Value = 0;
  while (fscanf(Native_File, "%lld", &Command_Value) > 0)
  {
    Proc->Code_Array[Proc->IP] = Command_Value;
    (Proc->IP)++;
    size_t Number_Arg = Cnt_Num_Arg_For_Command(Proc->Code_Array, Proc->IP - 1);
    if (Number_Arg == 1)
    {
      Elements Argument_Value = 0;
      if (fscanf(Native_File, "%lld", &Argument_Value) > 0)
      {
        Proc->Code_Array[Proc->IP] = Argument_Value;
        (Proc->IP)++;
      }
      else
      {
        printf("ERROR: syntax error\n");
        return ERROR_INVALID_ARGUMENT;
      }
    }
    else if (Number_Arg == 0)
      continue;
    else
    {
      printf("ERROR: incorrect number of arguments\n");
      return ERROR_INVALID_ARGUMENT;
    }
  }

  Proc->Code_Size = Proc->IP;
  return ERROR_NOT;
}*/

size_t Cnt_Num_Arg_For_Command(Elements Code_Array[], size_t i)
{
  Elements Val = Code_Array[i];
  if (Val == HLT || Val == ADD || Val == SUB || Val == MUL || Val == DIV ||
      Val == IN || Val == OUT || Val == RET || Val == DUMB || Val == SQRT || Val == SQU)
    return 0;
  else
    return 1;
}

int Doing_Commands_In_Code(Processor* Proc)
{
  Proc->IP = 0;
  while (Proc->IP < Proc->Code_Size)
  {
    Elements Command_Value = Proc->Code_Array[Proc->IP];
    bool Flag_Need_Exit = false, Jumped = false;
    int Temp = 0;
    switch (Command_Value)
    {
      case HLT: Flag_Need_Exit = Hlt_C(Flag_Need_Exit); break;

      case PUSH:  if (Push_C(Proc) != ERROR_NOT)  return ANSWER_POISON; break;

      case PUSHr: if (Pushr_C(Proc) != ERROR_NOT) return ANSWER_POISON; break;

      case POP:   if (Pop_C(Proc) != ERROR_NOT)   return ANSWER_POISON; break;

      case POPr:  if (Popr_C(Proc) != ERROR_NOT)  return ANSWER_POISON; break;

      case ADD:   if (Add_C(Proc) != ERROR_NOT)   return ANSWER_POISON; break;

      case SUB:   if (Sub_C(Proc) != ERROR_NOT)   return ANSWER_POISON; break;

      case MUL:   if (Mul_C(Proc) != ERROR_NOT)   return ANSWER_POISON; break;

      case DIV:   if (Div_C(Proc) != ERROR_NOT)   return ANSWER_POISON; break;

      case JMP: Jumped = Jmp_C(Proc); break;

      case JA:
      case JAE:
      case JB:
      case JBE:
      case JE:
      case JNE: Jumped = Jmp_Conditional_C(Proc, Command_Value); break;

      case IN:   if (In_C(Proc) != ERROR_NOT)   return ANSWER_POISON; break;

      case OUT:  if (Out_C(Proc) != ERROR_NOT)  return ANSWER_POISON; break;

      case CALL:
        Temp = Call_C(Proc);
        if   (Temp < 0)     return ANSWER_POISON;
        else Jumped = Temp; break;

      case RET:
        Temp = Ret_C(Proc);
        if   (Temp < 0)     return ANSWER_POISON;
        else Jumped = Temp; break;

      case DUMB: Dumb_C(Proc); break;

      case SQRT:   if (Sqrt_C(Proc) != ERROR_NOT)   return ANSWER_POISON; break;

      case SQU:    if (Squ_C(Proc) != ERROR_NOT)    return ANSWER_POISON; break;

      default: printf("ERROR: unsupported command\n"); break;
    }

    if (!Jumped)
    {
      size_t Number_Arg = Cnt_Num_Arg_For_Command(Proc->Code_Array, Proc->IP);
      Proc->IP += Number_Arg + 1;
    }

    if (Flag_Need_Exit)
      break;
  }

  return ERROR_NOT;
}

int Hlt_C(bool Flag)
{
  return !Flag;
}

Errors Push_C(Processor* Proc)
{
  if (Stack_Push(&(Proc->Stk), Proc->Code_Array[Proc->IP + 1]) != EVERYTHING_OK)
    return ERROR_DOING_ASM_COMMAND;
  return ERROR_NOT;
}

Errors Pushr_C(Processor* Proc)
{
  Elements Register_Value = Proc->Register_Array[Proc->Code_Array[Proc->IP + 1]];
  if (Stack_Push(&(Proc->Stk), Register_Value) != EVERYTHING_OK)
    return ERROR_DOING_ASM_COMMAND;
  return ERROR_NOT;
}

Errors Pop_C(Processor* Proc)
{
  Stack_Pop(&(Proc->Stk));
  return ERROR_NOT;
}

Errors Popr_C(Processor* Proc)
{
  Elements Value = Stack_Pop(&(Proc->Stk));
  Proc->Register_Array[Proc->Code_Array[Proc->IP + 1]] = Value;
  return ERROR_NOT;
}

Errors Add_C(Processor* Proc)
{
  Elements Value2 = Stack_Pop(&(Proc->Stk));
  Elements Value1 = Stack_Pop(&(Proc->Stk));
  if (Stack_Push(&(Proc->Stk), Value1 + Value2) != EVERYTHING_OK)
    return ERROR_DOING_ASM_COMMAND;
  return ERROR_NOT;
}

Errors Sub_C(Processor* Proc)
{
  Elements Value2 = Stack_Pop(&(Proc->Stk));
  Elements Value1 = Stack_Pop(&(Proc->Stk));
  if (Stack_Push(&(Proc->Stk), Value1 - Value2) != EVERYTHING_OK)
    return ERROR_DOING_ASM_COMMAND;
  return ERROR_NOT;
}

Errors Mul_C(Processor* Proc)
{
  Elements Value2 = Stack_Pop(&(Proc->Stk));
  Elements Value1 = Stack_Pop(&(Proc->Stk));
  if (Stack_Push(&(Proc->Stk), Value1 * Value2) != EVERYTHING_OK)
    return ERROR_DOING_ASM_COMMAND;
  return ERROR_NOT;
}

Errors Div_C(Processor* Proc)
{
  Elements Value2 = Stack_Pop(&(Proc->Stk));
  Elements Value1 = Stack_Pop(&(Proc->Stk));
  if (Value2 == 0 || Stack_Push(&(Proc->Stk), Value1 / Value2) != EVERYTHING_OK)
    return ERROR_DOING_ASM_COMMAND;
  return ERROR_NOT;
}

int Jmp_C(Processor* Proc)
{
  Proc->IP = (size_t)(Proc->Code_Array[Proc->IP + 1]);
  return 1;
}

int Jmp_Conditional_C(Processor* Proc, Elements Command)
{
  Elements Value2 = Stack_Pop(&(Proc->Stk));
  Elements Value1 = Stack_Pop(&(Proc->Stk));
  bool Flag_Compare = false;

  switch(Command)
  {
    case JA:  Flag_Compare = (Value1 >  Value2); break;
    case JAE: Flag_Compare = (Value1 >= Value2); break;
    case JB:  Flag_Compare = (Value1 <  Value2); break;
    case JBE: Flag_Compare = (Value1 <= Value2); break;
    case JE:  Flag_Compare = (Value1 == Value2); break;
    case JNE: Flag_Compare = (Value1 != Value2); break;

    default: break;
  }

  if (Flag_Compare)
    return Jmp_C(Proc);

  return 0;
}

Errors In_C(Processor* Proc)
{
  Elements Value = 0;
  if (scanf("%lld", &Value) <= 0)
    return ERROR_DOING_ASM_COMMAND;
  else
  {
    if (Stack_Push(&(Proc->Stk), Value) != EVERYTHING_OK)
      return ERROR_DOING_ASM_COMMAND;
  }
  return ERROR_NOT;
}

Errors Out_C(Processor* Proc)
{
  Elements Value = Stack_Pop(&(Proc->Stk));
  printf("%lld\n", Value);
  return ERROR_NOT;
}

int Call_C(Processor* Proc)
{
  Stack_Push(&(Proc->Ret_Addr), (Elements)(Proc->IP + 2));
  return Jmp_C(Proc);
}

int Ret_C(Processor* Proc)
{
  if (Proc->Ret_Addr.Size == 0)
    return ERROR_DOING_ASM_COMMAND;
  Elements Addr = Stack_Pop(&(Proc->Ret_Addr));
  Proc->IP = (size_t)Addr;
  return 1;
}

void Dumb_C(Processor* Proc)
{
  Code_Dumb(Proc);
  Registers_Dumb(Proc);
  Stack_Dumb(&(Proc->Stk));
  getchar();
  return;
}

Errors Sqrt_C(Processor* Proc)
{
  Elements Value = Stack_Pop(&(Proc->Stk));
  
  if (Stack_Push(&(Proc->Stk), (Elements)sqrt((double)Value)) != EVERYTHING_OK)
    return ERROR_DOING_ASM_COMMAND;
  return ERROR_NOT;
}

Errors Squ_C(Processor* Proc)
{
  Elements Value = Stack_Pop(&(Proc->Stk));
  
  if (Stack_Push(&(Proc->Stk), Value * Value) != EVERYTHING_OK)
    return ERROR_DOING_ASM_COMMAND;
  return ERROR_NOT;
}

void Code_Dumb(Processor* Proc)
{
  FILE* Error_File = fopen(FILE_FOR_ERRORS, "w");
  if (Error_File == NULL)
    printf("ERROR: can't open file for errors\n");
  else
  {
    fprintf(Error_File, "\n-------------------Code_Dumb---Start----------------\n");
    fprintf(Error_File, "Code_Size = %zu\n", Proc->Code_Size);
    fprintf(Error_File, "IP = %zu\n", Proc->IP);
    fprintf(Error_File, "Code_Array:\n");

    for (size_t i = 0; i < Proc->Code_Size; i++)
    {
      if (Proc->IP == i)
      {
        fprintf(Error_File, "[%2zu]:   [%lld]\n", i, Proc->Code_Array[i]);
      }
      else
      {
        fprintf(Error_File, " %2zu :    %lld\n", i, Proc->Code_Array[i]);
      }
    }

    fprintf(Error_File, "-------------------Code_Dumb---End----------------\n\n\n");

    if (fclose(Error_File) != 0)
      printf("WARNING: can't close file for errors\n");
  }

  return;
}

void Registers_Dumb(Processor* Proc)
{
  FILE* Error_File = fopen(FILE_FOR_ERRORS, "a");
  if (Error_File == NULL)
    printf("ERROR: can't open file for errors\n");
  else
  {
    fprintf(Error_File, "\n-------------------Registers_Dumb---Start----------------\n");
    fprintf(Error_File, "Register_Array:\n");

    for (size_t i = 0; i < MAX_REGISTERS_NUMBER; i++)
      fprintf(Error_File, "  %s  ", REGISTERS[i]);
    fprintf(Error_File, "\n");

    for (size_t i = 0; i < MAX_REGISTERS_NUMBER; i++)
      fprintf(Error_File, "  %-2lld  ", Proc->Register_Array[i]);
    fprintf(Error_File, "\n\n\n");

    fprintf(Error_File, "Register_Stack:");

    Stack_Dumb(&(Proc->Ret_Addr), Error_File);

    fprintf(Error_File, "\n-------------------Registers_Dumb---End----------------\n\n\n");

    if (fclose(Error_File) != 0)
      printf("WARNING: can't close file for errors\n");
  }

  return;
}