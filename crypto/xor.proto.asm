PRELUDE
MOV REG1, ARG1 --> address of target buf
MOV REG2, ARG2 --> size of target buf
MOV REG3, KEY

loop:
MOV REG4, [REG1]
XOR REG4, REG3
MOV [REG1], REG4
DEC REG2
ADD REG1
CMP REG2, 0
JNZ loop

EPILOGUE
RET
