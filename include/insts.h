#pragma once
#include <stdio.h>
#include <stdint.h>

typedef enum{
    INST_TYPE_PUSH,
    INST_TYPE_ADDI,
    INST_TYPE_ADDF,
    INST_TYPE_MINUSI,
    INST_TYPE_MINUSF,
    INST_TYPE_DUP,
    INST_TYPE_EQU,
    INST_TYPE_JMP,
    INST_TYPE_JMP_IF,
    INST_TYPE_HALT,
} Inst_Type;

typedef enum{
    BYTECODE_INST_TYPE_PUSH=0x10,
    BYTECODE_INST_TYPE_ADDI=0x19,
    BYTECODE_INST_TYPE_ADDF=0x21,
    BYTECODE_INST_TYPE_MINUSI=0x18,
    BYTECODE_INST_TYPE_MINUSF=0x20,
    BYTECODE_INST_TYPE_JMP=0x81,
    BYTECODE_INST_TYPE_JMP_IF=0x82,
    BYTECODE_INST_TYPE_EQU=0xe5,
    BYTECODE_INST_TYPE_DUP=0xd1,
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

char *inst_type_as_cstr(Inst_Type type){
    switch(type){
        case INST_TYPE_PUSH:   return "INST_TYPE_PUSH  ";
        case INST_TYPE_ADDI:    return "INST_TYPE_ADDI   ";
        case INST_TYPE_EQU:    return "INST_TYPE_EQU   ";
        case INST_TYPE_DUP:    return "INST_TYPE_DUP   ";
        case INST_TYPE_JMP:    return "INST_TYPE_JMP   ";
        case INST_TYPE_JMP_IF: return "INST_TYPE_JMP_IF";
        case INST_TYPE_MINUSI:  return "INST_TYPE_MINUSI ";
        case INST_TYPE_ADDF:  return "INST_TYPE_ADDF ";
        case INST_TYPE_MINUSF:  return "INST_TYPE_MINUSF ";
        case INST_TYPE_HALT:   return "INST_TYPE_HALT  ";
        default:               return NULL;
    }
    return NULL;
}

#define MAKE_INST(inst_type, argument) ((Inst) {.type = (inst_type), .operand.as_u64 = (argument)})