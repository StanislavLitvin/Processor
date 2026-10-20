#include "Virtual_Machine.h"

int main(int argc, char* argv[])
{
  if (argc < 2)
    printf("WARNING: too few command-line arguments\n");

  const char *Name_File_Native_Code = (argc >= 2) ? argv[1] : NAME_FILE_NATIVE_CODE_BY_DEFAULT;

  struct Processor Proc = {0};
  Proc.Code_Array = (int*)calloc(MAX_CODE_SIZE, sizeof(int));
  Proc.Register_Array = (int*)calloc(MAX_REGISTERS_NUMBER, sizeof(int));
  Proc.System_Register_Array = (int*)calloc(MAX_SYSTEM_REGISTERS_NUMBER, sizeof(int));

  FILE* Native_File = Fopen_File(Name_File_Native_Code, READING);
  if (Native_File == NULL)
    return -1;

  if (Read_File_And_Formating_Code_Array(Native_File, &Proc) != 0)
  {
    Fclose_File(Native_File, Name_File_Native_Code);
    return -2;
  }

  Clear_Log_File();
  Stack_Error_Status Err = Stack_Init(&Proc.Stk, START_CAPACITY ON_DBG(, "Stk", __FILE__, __LINE__));
  if (Err < 0)
  {
    printf("ERROR: memory allocation error\n");
    return Err;
  }

  if (Pass_Code_Array_And_Execution_Commands(&Proc) == ANSWER_POISON)
  {
    printf("ERROR: execution error\n");
    return -3;
  }

  if (Fclose_File(Native_File, Name_File_Native_Code) != 0)
    return -1;

  Stack_Destroy(&Proc.Stk);
  free(Proc.Code_Array);
  free(Proc.Register_Array);
  free(Proc.System_Register_Array);
  printf("Successful completion of the program\n");
  return 0;
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

int Fclose_File(FILE* File, const char* File_Name)
{
  if (File == NULL)
  {
    printf("ERROR: invalid pointer to file %s\n", File_Name);
    return 1;
  }

  if (fclose(File) == EOF)
    printf("WARNING: can't close file %s\n", File_Name);

  return 0;
}

int Read_File_And_Formating_Code_Array(FILE* Native_File, Processor* Proc)
{
  int Command_Value = 0;
  while (fscanf(Native_File, "%d", &Command_Value) > 0)
  {
    Proc->Code_Array[Proc->IP] = Command_Value;
    (Proc->IP)++;
    size_t Number_Arg = Count_Number_Arg_For_Command(Proc->Code_Array, Proc->IP - 1);
    if (Number_Arg == 1)
    {
      int Argument_Value = 0;
      if (fscanf(Native_File, "%d", &Argument_Value) > 0)
      {
        Proc->Code_Array[Proc->IP] = Argument_Value;
        (Proc->IP)++;
      }
      else
      {
        printf("ERROR: syntax error\n");
        return -1;
      }
    }
    else if (Number_Arg == 0)
      continue;
    else
    {
      printf("ERROR: incorrect number of arguments\n");
      return -1;
    }
  }

  Proc->Code_Size = Proc->IP;
  return 0;
}

size_t Count_Number_Arg_For_Command(int Code_Array[], size_t i)
{
  int Val = Code_Array[i];
  if (Val == HLT || Val == ADD || Val == SUB || Val == MUL ||
      Val == DIV || Val == IN || Val == OUT || Val == RET || Val == DUMB)
    return 0;
  else
    return 1;
}

int Pass_Code_Array_And_Execution_Commands(Processor* Proc)
{
  Proc->IP = 0;
  while (Proc->IP < Proc->Code_Size)
  {
    int Command_Value = Proc->Code_Array[Proc->IP];
    bool Flag_Need_Exit = false, Jumped = false;
    int Temp = 0;
    switch (Command_Value)
    {
      case HLT: Flag_Need_Exit = HLT_C(Flag_Need_Exit); break;

      case PUSH:  if (PUSH_C(Proc) != 0)  return ANSWER_POISON; break;

      case PUSHr: if (PUSHr_C(Proc) != 0) return ANSWER_POISON; break;

      case POP:   if (POP_C(Proc) != 0)   return ANSWER_POISON; break;

      case POPr:  if (POPr_C(Proc) != 0)  return ANSWER_POISON; break;

      case ADD:   if (ADD_C(Proc) != 0)   return ANSWER_POISON; break;

      case SUB:   if (SUB_C(Proc) != 0)   return ANSWER_POISON; break;

      case MUL:   if (MUL_C(Proc) != 0)   return ANSWER_POISON; break;

      case DIV:   if (DIV_C(Proc) != 0)   return ANSWER_POISON; break;

      case JMP: Jumped = JMP_C(Proc); break;

      case JA:  Jumped = JA_C(Proc);  break;

      case JAE: Jumped = JAE_C(Proc); break;

      case JB:  Jumped = JB_C(Proc);  break;

      case JBE: Jumped = JBE_C(Proc); break;

      case JE:  Jumped = JE_C(Proc);  break;

      case JNE: Jumped = JNE_C(Proc); break;

      case IN:   if (IN_C(Proc) != 0)   return ANSWER_POISON; break;

      case OUT:  if (OUT_C(Proc) != 0)  return ANSWER_POISON; break;

      case CALL:
        Temp = CALL_C(Proc);
        if   (Temp < 0)     return ANSWER_POISON;
        else Jumped = Temp; break;

      case RET:
        Temp = RET_C(Proc);
        if   (Temp < 0)     return ANSWER_POISON;
        else Jumped = Temp; break;

      case DUMB: DUMB_C(Proc); break;

      default: printf("ERROR: unsupported command\n"); break;
    }

    if (!Jumped)
    {
      size_t Number_Arg = Count_Number_Arg_For_Command(Proc->Code_Array, Proc->IP);
      Proc->IP += Number_Arg + 1;
    }

    if (Flag_Need_Exit)
      break;
  }

  return 0;
}

int HLT_C(bool Flag)
{
  return !Flag;
}

int PUSH_C(Processor* Proc)
{
  if (Stack_Push(&(Proc->Stk), Proc->Code_Array[Proc->IP + 1]) != EVERYTHING_OK)
    return -1;
  return 0;
}

int PUSHr_C(Processor* Proc)
{
  int Register_Value = Proc->Register_Array[Proc->Code_Array[Proc->IP + 1]];
  if (Stack_Push(&(Proc->Stk), Register_Value) != EVERYTHING_OK)
    return -1;
  return 0;
}

int POP_C(Processor* Proc)
{
  Stack_Pop(&(Proc->Stk));
  return 0;
}

int POPr_C(Processor* Proc)
{
  int Value = Stack_Pop(&(Proc->Stk));
  Proc->Register_Array[Proc->Code_Array[Proc->IP + 1]] = Value;
  return 0;
}

int ADD_C(Processor* Proc)
{
  int Value2 = Stack_Pop(&(Proc->Stk));
  int Value1 = Stack_Pop(&(Proc->Stk));
  if (Stack_Push(&(Proc->Stk), Value1 + Value2) != EVERYTHING_OK)
    return -1;
  return 0;
}

int SUB_C(Processor* Proc)
{
  int Value2 = Stack_Pop(&(Proc->Stk));
  int Value1 = Stack_Pop(&(Proc->Stk));
  if (Stack_Push(&(Proc->Stk), Value1 - Value2) != EVERYTHING_OK)
    return -1;
  return 0;
}

int MUL_C(Processor* Proc)
{
  int Value2 = Stack_Pop(&(Proc->Stk));
  int Value1 = Stack_Pop(&(Proc->Stk));
  if (Stack_Push(&(Proc->Stk), Value1 * Value2) != EVERYTHING_OK)
    return -1;
  return 0;
}

int DIV_C(Processor* Proc)
{
  int Value2 = Stack_Pop(&(Proc->Stk));
  int Value1 = Stack_Pop(&(Proc->Stk));
  if (Stack_Push(&(Proc->Stk), Value1 / Value2) != EVERYTHING_OK)
    return -1;
  return 0;
}

int JMP_C(Processor* Proc)
{
  Proc->IP = (size_t)(Proc->Code_Array[Proc->IP + 1]);
  return 1;
}

int JA_C(Processor* Proc)
{
  int Value1 = Stack_Pop(&(Proc->Stk));
  int Value2 = Stack_Pop(&(Proc->Stk));
  if (Value1 > Value2)
    return JMP_C(Proc);
  return 0;
}

int JAE_C(Processor* Proc)
{
  int Value1 = Stack_Pop(&(Proc->Stk));
  int Value2 = Stack_Pop(&(Proc->Stk));
  if (Value1 >= Value2)
    return JMP_C(Proc);
  return 0;
}

int JB_C(Processor* Proc)
{
  int Value1 = Stack_Pop(&(Proc->Stk));
  int Value2 = Stack_Pop(&(Proc->Stk));
  if (Value1 < Value2)
    return JMP_C(Proc);
  return 0;
}

int JBE_C(Processor* Proc)
{
  int Value1 = Stack_Pop(&(Proc->Stk));
  int Value2 = Stack_Pop(&(Proc->Stk));
  if (Value1 <= Value2)
    return JMP_C(Proc);
  return 0;
}

int JE_C(Processor* Proc)
{
  int Value1 = Stack_Pop(&(Proc->Stk));
  int Value2 = Stack_Pop(&(Proc->Stk));
  if (Value1 == Value2)
    return JMP_C(Proc);
  return 0;
}

int JNE_C(Processor* Proc)
{
  int Value1 = Stack_Pop(&(Proc->Stk));
  int Value2 = Stack_Pop(&(Proc->Stk));
  if (Value1 != Value2)
    return JMP_C(Proc);
  return 0;
}

int IN_C(Processor* Proc)
{
  int Value = 0;
  if (scanf("%d", &Value) <= 0)
    return -1;
  else
  {
    if (Stack_Push(&(Proc->Stk), Value) != EVERYTHING_OK)
      return -1;
  }
  return 0;
}

int OUT_C(Processor* Proc)
{
  int Value = Stack_Pop(&(Proc->Stk));
  printf("%d\n", Value);
  return 0;
}

int CALL_C(Processor* Proc)
{
  if (Proc->Free_System_Register >= MAX_SYSTEM_REGISTERS_NUMBER)
    return -1;
  Proc->System_Register_Array[Proc->Free_System_Register] = (int)(Proc->IP + 2);
  Proc->Free_System_Register++;
  return JMP_C(Proc);
}

int RET_C(Processor* Proc)
{
  if (Proc->Free_System_Register == 0)
    return -1;
  Proc->Free_System_Register--;
  int Addr = Proc->System_Register_Array[Proc->Free_System_Register];
  Proc->IP = (size_t)Addr;
  return 1;
}

void DUMB_C(Processor* Proc)
{
  Stack_Dumb(&(Proc->Stk));
  Code_Dumb(Proc);
  Registers_Dumb(Proc);
  getchar();
  return;
}

void Code_Dumb(Processor* Proc)
{
  FILE* Error_File = fopen(FILE_FOR_ERRORS, "w");
  if (Error_File == NULL)
    printf("ERROR: can't open file for errors\n");
  else
  {
    fprintf(Error_File, "\n-------------------Code_Dumb---Start----------------\n\n");
    fprintf(Error_File, "Code_Size = %zu\n", Proc->Code_Size);
    fprintf(Error_File, "IP = %zu\n", Proc->IP);
    fprintf(Error_File, "Code_Array:\n");

    for (size_t i = 0; i < Proc->Code_Size; i++)
    {
      if (Proc->IP == i)
        fprintf(Error_File, " [%zu] ", i);
      else
        fprintf(Error_File, "  %zu  ", i);
    }
    fprintf(Error_File, "\n");

    for (size_t i = 0; i < Proc->Code_Size; i++)
    {
      if (Proc->IP == i)
        fprintf(Error_File, " [%d] ", Proc->Code_Array[i]);
      else
        fprintf(Error_File, "  %d  ", Proc->Code_Array[i]);
    }
    fprintf(Error_File, "\n");

    for (size_t i = 0; i < Proc->Code_Size; i++)
    {
      if (Proc->IP != i)
        fprintf(Error_File, "     ");
      else
        fprintf(Error_File, "  ^  ");
    }
    fprintf(Error_File, "\n");

    fprintf(Error_File, "\n-------------------Code_Dumb---End----------------\n\n");

    if (fclose(Error_File) != 0)
      printf("WARNING: can't close file for errors\n");
  }
}

void Registers_Dumb(Processor* Proc)
{
  FILE* Error_File = fopen(FILE_FOR_ERRORS, "a");
  if (Error_File == NULL)
    printf("ERROR: can't open file for errors\n");
  else
  {
    fprintf(Error_File, "\n-------------------Registers_Dumb---Start----------------\n\n");
    fprintf(Error_File, "Register_Array:\n");

    for (size_t i = 0; i < MAX_REGISTERS_NUMBER; i++)
      fprintf(Error_File, "  %s  ", REGISTERS[i]);
    fprintf(Error_File, "\n");

    for (size_t i = 0; i < MAX_REGISTERS_NUMBER; i++)
      fprintf(Error_File, "  %d   ", Proc->Register_Array[i]);
    fprintf(Error_File, "\n\n\n");

    fprintf(Error_File, "Free_System_Register = %zu\n", Proc->Free_System_Register);
    fprintf(Error_File, "System_Register_Array:\n");

    for (size_t i = 0; i < MAX_SYSTEM_REGISTERS_NUMBER; i++)
      fprintf(Error_File, "  %s  ", SYSTEM_REGISTERS[i]);
    fprintf(Error_File, "\n");

    for (size_t i = 0; i < MAX_SYSTEM_REGISTERS_NUMBER; i++)
      fprintf(Error_File, "   %d   ", Proc->System_Register_Array[i]);
    fprintf(Error_File, "\n");

    fprintf(Error_File, "\n-------------------Registers_Dumb---End----------------\n\n");

    if (fclose(Error_File) != 0)
      printf("WARNING: can't close file for errors\n");
  }
}