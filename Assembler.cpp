#include <stdio.h>
#include <string.h>

const int MAX_LEN_STR_ASM = 100;
const char* NAME_FILE_ASM_SOURCE_BY_DEFAULT = "Code.ASM";
const char* NAME_FILE_NATIVE_CODE_BY_DEFAULT = "Code.txt";

enum Comands
{
  HLT  = 0,
  PUSH = 1,
  ADD  = 2,
  SUB  = 3,
  DIV  = 4,
  OUT  = 5,
  DUMB = 6
};

int main(int argc, char* argv[])
{
  const char *Name_File_ASM_Source = (argc >= 2) ? argv[1] : NAME_FILE_ASM_SOURCE_BY_DEFAULT;
  const char *Name_File_Native_Code = (argc >= 3) ? argv[2] : NAME_FILE_NATIVE_CODE_BY_DEFAULT;

  FILE* ASM_Source = fopen(Name_File_ASM_Source, "r");
  if (ASM_Source == NULL)
  {
    printf("ERROR: cannot open file ASM\n");
    return 1;
  }

  FILE* Native_Code = fopen(Name_File_Native_Code, "w");//в отдельную фунецию закрытие тоже
  if (Native_Code == NULL)
  {
    printf("ERROR: cannot open file Native\n");
    return 1;
  }

  char Str[MAX_LEN_STR_ASM] = {}, Comand[MAX_LEN_STR_ASM] = {};//дин память
  double Argument = 0;
  while(fgets(Str, MAX_LEN_STR_ASM, ASM_Source) != NULL)
  {
    if (sscanf(Str, "%s", Comand) != 1)//буффер
      continue;

    if (!strcmp(Comand, "PUSH"))
    {
      if (sscanf(Str, "%*s %lg", &Argument) != 1)//scanf до \n почитать
      {
        printf("ERROR: incorrect comand PUSH in %s", Str);
        break;
      }
      fprintf(Native_Code, "%d %lg\n", PUSH, Argument);
    }
    else if (!strcmp(Comand, "ADD"))
    {
      fprintf(Native_Code, "%d\n", ADD);
    }
    else if (!strcmp(Comand, "SUB"))
    {
      fprintf(Native_Code, "%d\n", SUB);
    }
    else if (!strcmp(Comand, "DIV"))
    {
      fprintf(Native_Code, "%d\n", DIV);
    }
    else if (!strcmp(Comand, "OUT"))
    {
      fprintf(Native_Code, "%d\n", OUT);
    }
    else if (!strcmp(Comand, "HLT"))
    {
      fprintf(Native_Code, "%d", HLT);
    }
    else if (!strcmp(Comand, "DUMB"))
    {
      fprintf(Native_Code, "%d\n", DUMB);
    }
    else
    {
      printf("ERROR: unknown command '%s'\n", Comand);
      break;
    }
  }

  fclose(ASM_Source);//проверки
  fclose(Native_Code);
  return 0;
}