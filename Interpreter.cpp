#include <stdio.h>
#include <string.h>
#include "Stack.h"
#include "Functions.cpp"

const int MAX_LEN_STR_ASM = 100;

int main()
{
  Clear_Log_File();

  Stack_t Stk = {};

  Stack_Error_Status Err = Stack_Init(&Stk, STACK_START_NUMBERS_ELEM
                                      ON_DBG(, "Stk", __FILE__, __LINE__));
  if (Err < 0)
  {
    printf(RED "ERROR: memory allocation error\n" RESET);
    return Err;
  }

  FILE* ASM_Source = fopen("Code.ASM", "r");
  if (ASM_Source == NULL)
  {
    printf(RED "ERROR: cannot open file Code.ASM\n" RESET);
    Stack_Destroy(&Stk);
    return 1;
  }

  char Str[MAX_LEN_STR_ASM] = {}, Comand[MAX_LEN_STR_ASM] = {};
  Stack_Elem_t Argument = 0;

  while(fgets(Str, MAX_LEN_STR_ASM, ASM_Source) != NULL)
  {
    sscanf(Str, "%s", Comand);
    if (!strcmp(Comand, "PUSH"))
    {
      if (sscanf(Str, "%*s %lg", &Argument) != 1)
      {
        printf("ERROR: incorrect comand PUSH in %s", Str);
        break;
      }
      Stack_Push(&Stk, Argument);
    }
    else if (!strcmp(Comand, "ADD"))
    {
      Stack_Elem_t Value2 = Stack_Pop(&Stk);
      Stack_Elem_t Value1 = Stack_Pop(&Stk);
      Stack_Push(&Stk, Value1 + Value2);
    }
    else if (!strcmp(Comand, "SUB"))
    {
      Stack_Elem_t Value2 = Stack_Pop(&Stk);
      Stack_Elem_t Value1 = Stack_Pop(&Stk);
      Stack_Push(&Stk, Value1 - Value2);
    }
    else if (!strcmp(Comand, "DIV"))
    {
      Stack_Elem_t Value2 = Stack_Pop(&Stk);
      Stack_Elem_t Value1 = Stack_Pop(&Stk);
      Stack_Push(&Stk, Value1 / Value2);
    }
    else if (!strcmp(Comand, "OUT"))
    {
      Stack_Elem_t Value = Stack_Pop(&Stk);
      printf("%lg\n", Value);
    }
    else if (!strcmp(Comand, "HLT"))
    {
      break;
    }
    else if (!strcmp(Comand, "DUMB"))
    {
      Stack_Dumb(&Stk);
    }
    else
    {
      printf("ERROR");
      break;
    }
  }

  Stack_Destroy(&Stk);

  if (fclose(ASM_Source) != 0)
    printf("WARNING: File didn't close successfully");

  return 0;
}