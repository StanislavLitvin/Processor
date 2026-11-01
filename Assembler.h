typedef long long int Elements; //если вдруг не работает написанный код на асме, то первое - посмотри выходы за разные константы размеров чего-то

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "Commands_Registers_Errors.h"

#define MAX_CODE_SIZE 1000
#define MAX_LABELS_NUMBER 20
#define MAX_REGISTERS_NUMBER 10
#define MAX_COMMAND_LEN 10
#define MAX_ARGUMENT_LEN 10
#define MAX_LABEL_LEN 10

const char* NAME_FILE_ASM_SOURCE_BY_DEFAULT = "Code.ASM";
const char* NAME_FILE_NATIVE_CODE_BY_DEFAULT = "Code.txt";
const char* READING = "r";
const char* WRITING = "w";
const char* REGISTERS[MAX_REGISTERS_NUMBER] = {"AX", "BX", "CX", "DX", "SI",
                                               "DI", "BP", "SP", "R8", "R9"};
const int POISON_FOR_LABELS = -(MAX_LABELS_NUMBER + 1);

int Open_File(const char* File_Name);
FILE* Fopen_File(const char* File_Name, const char* Mode);
Errors Close_File(int Descriptor, const char* File_Name);
Errors Fclose_File(FILE* File, const char* File_Name);
size_t Read_From_ASM_To_Buffer(int ASM_Source_Descriptor, char** Buffer);
Errors First_Assemblering(char* Buffer, Elements Code_Array[], size_t* Code_Size, int Labels_Array[]);
Commands Get_Command_Value(char* Command_Name);
Registers Get_Argument_Value(char* Argument_Name);
int Extract_Number_From_Label(char* Command_Name);
bool Is_In_REGISTERS(char* Argument_Name);
Errors Second_Assemblering(Elements Code_Array[], size_t Code_Size, int Labels_Array[]);
void Reset_Labels(int Labels_Array[]);
Errors Print_To_File(Elements Code_Array[], size_t Code_Size, FILE* Native_File);
size_t Cnt_Num_Arg_For_Command(Elements Code_Array[], size_t i);
bool Need_Label(Elements Command_Value);