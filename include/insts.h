#pragma once
#include <stdio.h>
#include <stdint.h>
#define VERSION 0, 5, 0

typedef enum{
    INST_TYPE_PUSH=1,
    INST_TYPE_ADDI,
    INST_TYPE_ADDF,
    INST_TYPE_MINUSI,
    INST_TYPE_MINUSF,
    INST_TYPE_DUP,
    INST_TYPE_EQU,
    INST_TYPE_JMP,
    INST_TYPE_JMP_IF,
    INST_TYPE_CALL,
    INST_TYPE_RET,
    INST_TYPE_DROP,
    INST_TYPE_SWAP,
    INST_TYPE_NATIVE,
    INST_TYPE_PRINTSTR,
    INST_TYPE_INTTOFLOAT,
    INST_TYPE_HALT,
} Inst_Type;

typedef enum{
    BYTECODE_INST_TYPE_PUSH=1,
    BYTECODE_INST_TYPE_ADDI,
    BYTECODE_INST_TYPE_ADDF,
    BYTECODE_INST_TYPE_MINUSI,
    BYTECODE_INST_TYPE_MINUSF,
    BYTECODE_INST_TYPE_JMP,
    BYTECODE_INST_TYPE_RET,
    BYTECODE_INST_TYPE_CALL,
    BYTECODE_INST_TYPE_DROP,
    BYTECODE_INST_TYPE_JMP_IF,
    BYTECODE_INST_TYPE_EQU,
    BYTECODE_INST_TYPE_DUP,
    BYTECODE_INST_TYPE_SWAP,
    BYTECODE_INST_TYPE_NATIVE,
    BYTECODE_INST_TYPE_PRINTSTR,
    BYTECODE_INST_TYPE_INTTOFLOAT,
    BYTECODE_STR_ID=0xFE,
    BYTECODE_INST_TYPE_HALT=0xFF,
} Bytecode_Inst_Type;

typedef union{
    uint64_t as_u64;
    int64_t as_i64;
    double as_f64;
    void *as_ptr;
} Operand;

typedef struct{
    Inst_Type type;
    Operand operand;
} Inst;
#define MAKE_INST(inst_type, argument) ((Inst) {.type = (inst_type), .operand.as_u64 = (argument)})

char *inst_type_as_cstr(Inst_Type type);