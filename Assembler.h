#include <stdio.h> //сделать массив адресов возврата, а не системные регистры
#include <stdlib.h> //если вдруг не работает написанный код на асме, то первое - посмотри выходы за разные константы размеров чего-то
#include <string.h> //сделать нормальный дамп, чтобы все четко совпадало по вертикали
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "Commands_And_Registers.h"

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
int Close_File(int Descriptor, const char* File_Name);
int Fclose_File(FILE* File, const char* File_Name);
size_t Read_From_ASM_To_Buffer(int ASM_Source_Descriptor, char** Buffer);
int First_Pass_Assemblering(char* Buffer, int Code_Array[], size_t* Code_Size, int Labels_Array[]);
Commands Get_Command_Value(char* Command_Name);
Registers Get_Argument_Value(char* Argument_Name);
int Extract_Number_From_Label(char* Command_Name);
bool Is_Contained_In_REGISTERS(char* Argument_Name);
int Second_Pass_Assemblering(int Code_Array[], size_t Code_Size, int Labels_Array[]);
void Reset_Labels_Array(int Labels_Array[]);
int Read_From_Code_Array_To_Native_File(int Code_Array[], size_t Code_Size, FILE* Native_File);
size_t Count_Number_Arg_For_Command(int Code_Array[], size_t i);