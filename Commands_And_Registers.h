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
  DUMB  = 20
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