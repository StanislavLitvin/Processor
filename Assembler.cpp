#include "Assembler.h"

int main(int argc, char* argv[])
{
  if (argc < 3)
    printf("WARNING: too few command-line arguments\n");

  const char *Name_File_ASM_Source = (argc >= 2) ? argv[1] : NAME_FILE_ASM_SOURCE_BY_DEFAULT;
  const char *Name_File_Native_Code = (argc >= 3) ? argv[2] : NAME_FILE_NATIVE_CODE_BY_DEFAULT;

  int ASM_Source_Descriptor = Open_File(Name_File_ASM_Source);
  if (ASM_Source_Descriptor == ERROR_OPENING_FILE)
    return ERROR_OPENING_FILE;

  char* Buffer = NULL;
  size_t Buffer_Size_In_Bytes = Read_From_ASM_To_Buffer(ASM_Source_Descriptor, &Buffer);

  if (Close_File(ASM_Source_Descriptor, Name_File_ASM_Source) != ERROR_NOT)
    return ERROR_CLOSING_FILE;

  if (Buffer_Size_In_Bytes == ERROR_NOT)
  {
    printf("ERROR: invalid buffer creation\n");
    return ERROR_INVALID_BUFFER;
  }

  Elements Code_Array[MAX_CODE_SIZE] = {0};
  size_t Code_Size = 0;
  int Labels_Array[MAX_LABELS_NUMBER] = {0};
  Reset_Labels(Labels_Array);

  if (First_Assemblering(Buffer, Code_Array, &Code_Size, Labels_Array) != ERROR_NOT)
  {
    printf("ERROR: first pass was failed\n");
    free(Buffer);
    return ERROR_FIRST_PASS;
  }

  if (Second_Assemblering(Code_Array, Code_Size, Labels_Array) != ERROR_NOT)
  {
    printf("ERROR: second pass was failed\n");
    free(Buffer);
    return ERROR_SECOND_PASS;
  }

  free(Buffer);

  /*printf("Code_Size = %zu\n", Code_Size);
  for (size_t i = 0; i < Code_Size; i++)
    printf("Code_Array[%zu] = %d\n", i, Code_Array[i]);*/

  FILE* Native_File = Fopen_File(Name_File_Native_Code, WRITING);
  if (Native_File == NULL)
    return ERROR_OPENING_FILE;

  if (Print_To_File(Code_Array, Code_Size, Native_File) != ERROR_NOT)
    return ERROR_PRINT_TO_FILE;

  if (Fclose_File(Native_File, Name_File_Native_Code) != ERROR_NOT)
    return ERROR_CLOSING_FILE;

  printf("Successful completion of the program\n");
  return ERROR_NOT;
}

int Open_File(const char* File_Name)
{
  int Descriptor = open(File_Name, O_RDONLY);
  if (Descriptor < 0)
  {
    printf("ERROR: can't open file %s\n", File_Name);
    return ERROR_OPENING_FILE;
  }

  return Descriptor;
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

Errors Close_File(int Descriptor, const char* File_Name)
{
  if (Descriptor < 0)
  {
    printf("ERROR: invalid descriptor to file %s\n", File_Name);
    return ERROR_CLOSING_FILE;
  }

  if (close(Descriptor) != 0)
    printf("WARNING: can't close file %s\n", File_Name);

  return ERROR_NOT;
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

size_t Read_From_ASM_To_Buffer(int ASM_Source_Descriptor, char** Buffer)
{
  struct stat Stats;
  if (fstat(ASM_Source_Descriptor, &Stats) != 0)
  {
    printf("ERROR: fstat failed\n");
    return ERROR_NOT;
  }

  size_t File_Size = (size_t)(Stats.st_size);
  *Buffer = (char*)calloc(File_Size + 1, sizeof(char));
  if (*Buffer == NULL)
  {
    printf("ERROR: calloc memory failed\n");
    return ERROR_NOT;
  }

  ssize_t Bytes_Number = read(ASM_Source_Descriptor, *Buffer, (unsigned int)File_Size);

  if (Bytes_Number <= 0)
  {
    free(*Buffer);
    return ERROR_NOT;
  }

  (*Buffer)[Bytes_Number] = '\0';
  
  return (size_t)Bytes_Number;
}

Errors First_Assemblering(char* Buffer, Elements Code_Array[], size_t* Code_Size,
                            int Labels_Array[])
{
  char Command_Name[MAX_COMMAND_LEN] = {0};
  char Argument_Name[MAX_ARGUMENT_LEN] = {0};
  size_t Current_Address = 0;
  int Increase = 0;
  size_t Counter_Commands = 0;

  while (sscanf(Buffer, "%s%n", Command_Name, &Increase) == 1 &&
         Current_Address < MAX_CODE_SIZE)
  {
    Counter_Commands++;
    Buffer += Increase;

    if (Command_Name[0] == ':')
    {
      int Label_Argument = Extract_Number_From_Label(Command_Name);
      if (Label_Argument <= 0 || Label_Argument >= MAX_LABELS_NUMBER)
      {
        printf("ERROR: syntax error (invalid ladel number)\n");
        return ERROR_INVALID_LABEL;
      }
      if (Labels_Array[Label_Argument - 1] != POISON_FOR_LABELS)
      {
        printf("ERROR: syntax error (used the same label names)\n");
        return ERROR_INVALID_LABEL;
      }

      Labels_Array[Label_Argument - 1] = (int)Current_Address;
    }
    else
    {
      Commands Command_Value = Get_Command_Value(Command_Name);
      if (Command_Value == ERROR)
      {
        printf("ERROR: syntax error (undefined command name)\n");
        return ERROR_INVALID_COMMAND;
      }

      Code_Array[Current_Address] = (Elements)Command_Value;
      Current_Address++;

      if (Cnt_Num_Arg_For_Command(Code_Array, Current_Address - 1) == 1)
      {
        if (sscanf(Buffer, "%s%n", Argument_Name, &Increase) != 1)
        {
          printf("ERROR: syntax error (the command argument could not be read)\n");
          return ERROR_INVALID_COMMAND;
        }

        Buffer += Increase;

        Elements Argument_Value = 0;
        int Chars_Read = 0;
        if (sscanf(Argument_Name, "%lld%n", &Argument_Value, &Chars_Read) == 1 &&
          Argument_Name[Chars_Read] == '\0')
        {
          Code_Array[Current_Address] = Argument_Value;
          Current_Address++;
        }
        else if (Argument_Name[0] == ':')
        {
          int Label_Argument = Extract_Number_From_Label(Argument_Name);
          if (Label_Argument < 0 || Label_Argument >= MAX_LABELS_NUMBER)
          {
            printf("ERROR: syntax error (invalid label number)\n");
            return ERROR_INVALID_LABEL;
          }

          if (Labels_Array[Label_Argument - 1] == POISON_FOR_LABELS)
            Code_Array[Current_Address] = (Elements)((-1) * (Label_Argument));
          else
            Code_Array[Current_Address] = (Elements)Labels_Array[Label_Argument - 1];
          Current_Address++;
        }
        else if (Is_In_REGISTERS(Argument_Name))
        {
          Registers Register_Value = Get_Argument_Value(Argument_Name);
          if (Register_Value == ER)
          {
            printf("ERROR: syntax error (undefined register name)\n");
            return ERROR_INVALID_REGISTER;
          }

          Code_Array[Current_Address] = Register_Value;
          Current_Address++;
        }
        else
        {
          printf("ERROR: syntax error (the argument of command is incorrect)\n");
          printf("ERROR: %zu-com %s %s\n", Counter_Commands, Command_Name, Argument_Name);
          return ERROR_INVALID_ARGUMENT;
        }
      }
    }
  }

  *Code_Size = Current_Address;
  return ERROR_NOT;
}

Commands Get_Command_Value(char* Command_Name)
{
  if (strcmp(Command_Name, "HLT")   == 0) return HLT;
  if (strcmp(Command_Name, "PUSH")  == 0) return PUSH;
  if (strcmp(Command_Name, "PUSHr") == 0) return PUSHr;
  if (strcmp(Command_Name, "POP")   == 0) return POP;
  if (strcmp(Command_Name, "POPr")  == 0) return POPr;
  if (strcmp(Command_Name, "ADD")   == 0) return ADD;
  if (strcmp(Command_Name, "SUB")   == 0) return SUB;
  if (strcmp(Command_Name, "MUL")   == 0) return MUL;
  if (strcmp(Command_Name, "DIV")   == 0) return DIV;
  if (strcmp(Command_Name, "JMP")   == 0) return JMP;
  if (strcmp(Command_Name, "JA")    == 0) return JA;
  if (strcmp(Command_Name, "JAE")   == 0) return JAE;
  if (strcmp(Command_Name, "JB")    == 0) return JB;
  if (strcmp(Command_Name, "JBE")   == 0) return JBE;
  if (strcmp(Command_Name, "JE")    == 0) return JE;
  if (strcmp(Command_Name, "JNE")   == 0) return JNE;
  if (strcmp(Command_Name, "IN")    == 0) return IN;
  if (strcmp(Command_Name, "OUT")   == 0) return OUT;
  if (strcmp(Command_Name, "CALL")  == 0) return CALL;
  if (strcmp(Command_Name, "RET")   == 0) return RET;
  if (strcmp(Command_Name, "DUMB")  == 0) return DUMB;
  if (strcmp(Command_Name, "SQRT")  == 0) return SQRT;
  if (strcmp(Command_Name, "SQU")   == 0) return SQU;
  
  return ERROR;
}

Registers Get_Argument_Value(char* Argument_Name)
{
  if (strcmp(Argument_Name, "AX") == 0) return AX;
  if (strcmp(Argument_Name, "BX") == 0) return BX;
  if (strcmp(Argument_Name, "CX") == 0) return CX;
  if (strcmp(Argument_Name, "DX") == 0) return DX;
  if (strcmp(Argument_Name, "SI") == 0) return SI;
  if (strcmp(Argument_Name, "DI") == 0) return DI;
  if (strcmp(Argument_Name, "BP") == 0) return BP;
  if (strcmp(Argument_Name, "SP") == 0) return SP;
  if (strcmp(Argument_Name, "R8") == 0) return R8;
  if (strcmp(Argument_Name, "R9") == 0) return R9;

  return ER;
}

int Extract_Number_From_Label(char* Command_Name)
{
  int i = 0;
  char Str_Command_Value[MAX_LABEL_LEN] = {0};
  while (Command_Name[i + 1] != '\0')
  {
    Str_Command_Value[i] = Command_Name[i + 1];
    i++;
  }
  Str_Command_Value[i] = '\0';
  
  int Ans = 0;
  int Chars_Read = 0;
  if (sscanf(Str_Command_Value, "%d%n", &Ans, &Chars_Read) != 1 ||
             Str_Command_Value[Chars_Read] != '\0')
  {
    printf("ERROR: syntax error (invalid label name)\n");
    return ERROR_INVALID_LABEL;
  }

  return Ans;
}

bool Is_In_REGISTERS(char* Argument_Name)
{
  bool Flag = false;
  for (size_t i = 0; i < MAX_REGISTERS_NUMBER; i++)
  {
    if (strcmp(Argument_Name, REGISTERS[i]) == 0)
      Flag = true;
  }

  return Flag;
}

Errors Second_Assemblering(Elements Code_Array[], size_t Code_Size, int Labels_Array[])
{
  size_t i = 0;
  while (i < Code_Size)
  {
    if (Cnt_Num_Arg_For_Command(Code_Array, i) == 1)
    {
      if (Code_Array[i + 1] < 0 && Need_Label(Code_Array[i]))
      {
        Elements Lab = (-1) * (Code_Array[i + 1]);
        if (Lab >= 1 && Lab < MAX_LABELS_NUMBER && Labels_Array[Lab - 1] != POISON_FOR_LABELS)
          Code_Array[i + 1] = (Elements)Labels_Array[Lab - 1];
        else
        {
          printf("ERROR: undefined label :%lld\n", Lab);
          return ERROR_INVALID_LABEL;
        }
      }
      i += 2;
    }
    else
      i += 1;
  }

  return ERROR_NOT;
}

void Reset_Labels(int Labels_Array[])
{
  for (size_t i = 0; i < MAX_LABELS_NUMBER; i++)
  {
    Labels_Array[i] = POISON_FOR_LABELS;
  }

  return;
}

Errors Print_To_File(Elements Code_Array[], size_t Code_Size,
                                        FILE* Native_File)
{
  size_t i = 0;
  while (i < Code_Size)
  {
    size_t Number_Arg_For_Command = Cnt_Num_Arg_For_Command(Code_Array, i);

    fprintf(Native_File, "%lld", Code_Array[i]);
    if (Number_Arg_For_Command == 0)
      fprintf(Native_File, "\n");
    else if (Number_Arg_For_Command == 1)
      fprintf(Native_File, " %lld\n", Code_Array[i + 1]);
    else
    {
      printf("ERROR: syntax error (incorrect number of arguments)\n");
      return ERROR_INVALID_ARGUMENT;
    }

    i += 1 + Number_Arg_For_Command;
  }

  return ERROR_NOT;
}

size_t Cnt_Num_Arg_For_Command(Elements Code_Array[], size_t i)
{
  Elements Val = Code_Array[i];
  if (Val == HLT || Val == ADD || Val == SUB || Val == MUL || Val == DIV ||
      Val == IN || Val == OUT || Val == RET || Val == DUMB || Val == SQRT || Val == SQU)
    return 0;
  else
    return 1;
}

bool Need_Label(Elements Command_Value)
{
  if (Command_Value == JMP || Command_Value == JA || Command_Value == JAE ||
      Command_Value == JB || Command_Value == JBE || Command_Value == JE ||
      Command_Value == JNE || Command_Value == CALL)
    return true;

  return false;
}