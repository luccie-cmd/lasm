#include "insts.h"

char *inst_type_as_cstr(Inst_Type type){
    switch(type){
        case INST_TYPE_PUSH:    return "INST_TYPE_PUSH  ";
        case INST_TYPE_ADDI:    return "INST_TYPE_ADDI  ";
        case INST_TYPE_SWAP:    return "INST_TYPE_SWAP  ";
        case INST_TYPE_CALL:    return "INST_TYPE_CALL  ";
        case INST_TYPE_DROP:    return "INST_TYPE_DROP  ";
        case INST_TYPE_EQU:     return "INST_TYPE_EQU   ";
        case INST_TYPE_DUP:     return "INST_TYPE_DUP   ";
        case INST_TYPE_JMP:     return "INST_TYPE_JMP   ";
        case INST_TYPE_RET:     return "INST_TYPE_RET   ";
        case INST_TYPE_JMP_IF:  return "INST_TYPE_JMP_IF";
        case INST_TYPE_MINUSI:  return "INST_TYPE_MINUSI";
        case INST_TYPE_ADDF:    return "INST_TYPE_ADDF  ";
        case INST_TYPE_MINUSF:  return "INST_TYPE_MINUSF";
        case INST_TYPE_NATIVE:  return "INST_TYPE_NATIVE";
        case INST_TYPE_PRINTSTR:  return "INST_TYPE_PRINTSTR";
        case INST_TYPE_INTTOFLOAT:  return "INST_TYPE_INTTOFLOAT";
        case INST_TYPE_HALT:    return "INST_TYPE_HALT  ";
        default:                return NULL;
    }
    return NULL;
}