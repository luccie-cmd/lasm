#include "insts.h"

char *inst_type_as_cstr(Inst_Type type){
    switch(type){
        case INST_TYPE_PUSH:    return "INST_TYPE_PUSH  ";
        case INST_TYPE_ADDI:    return "INST_TYPE_ADDI   ";
        case INST_TYPE_EQU:     return "INST_TYPE_EQU   ";
        case INST_TYPE_DUP:     return "INST_TYPE_DUP   ";
        case INST_TYPE_JMP:     return "INST_TYPE_JMP   ";
        case INST_TYPE_JMP_IF:  return "INST_TYPE_JMP_IF";
        case INST_TYPE_MINUSI:  return "INST_TYPE_MINUSI ";
        case INST_TYPE_ADDF:    return "INST_TYPE_ADDF ";
        case INST_TYPE_MINUSF:  return "INST_TYPE_MINUSF ";
        case INST_TYPE_HALT:    return "INST_TYPE_HALT  ";
        default:                return NULL;
    }
    return NULL;
}