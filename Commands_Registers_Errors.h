enum Commands
{
  ERROR = -1,
  HLT   =  0,
  PUSH  =  1,
  PUSHr =  2,
  POP   =  3,
  POPr  =  4,
  ADD   =  5,
  SUB   =  6,
  MUL   =  7,
  DIV   =  8,
  JMP   =  9,
  JA    = 10,
  JAE   = 11,
  JB    = 12,
  JBE   = 13,
  JE    = 14,
  JNE   = 15,
  IN    = 16,
  OUT   = 17,
  CALL  = 18,
  RET   = 19,
  DUMB  = 20,
  SQRT  = 21,
  SQU   = 22
};

enum Registers
{
  ER = -1,
  AX =  0,
  BX =  1,
  CX =  2,
  DX =  3,
  SI =  4,
  DI =  5,
  BP =  6,
  SP =  7,
  R8 =  8,
  R9 =  9
};

enum Errors
{
  ERROR_NOT               =   0,
  ERROR_OPENING_FILE      =  -1,
  ERROR_CLOSING_FILE      =  -2,
  ERROR_INVALID_BUFFER    =  -3,
  ERROR_FIRST_PASS        =  -4,
  ERROR_SECOND_PASS       =  -5,
  ERROR_PRINT_TO_FILE     =  -6,
  ERROR_INVALID_LABEL     =  -7,
  ERROR_INVALID_COMMAND   =  -8,
  ERROR_INVALID_REGISTER  =  -9,
  ERROR_INVALID_ARGUMENT  = -10,
  ERROR_DOING             = -11,
  ERROR_DOING_ASM_COMMAND = -12,
  ERROR_ALLOCATION        = -13
};