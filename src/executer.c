#include <stdio.h>
#include <stdlib.h>
#include "insts.h"

typedef struct{
    Operand stack[1024];
    uint64_t stack_size;

    Inst program[1024];
    uint64_t programLen;

    uint64_t ip;
    int halt;
} Executer;

uint64_t *readFile(const char *filePath, size_t *len){
    FILE *f = fopen(filePath, "rb");
    if(!f){
        printf("No file or directory named %s\n", filePath);
        return NULL;
    }
    
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);

    uint64_t *buffer = malloc(size/8);
    size_t lenRead = 0;
    if((lenRead = fread(buffer, 1, size, f)) == 0){
        printf("Could not read any data\n");
        free(buffer);
        return NULL;
    }
    buffer[lenRead] = 0x00;
    *len = lenRead/8;
    return buffer;
}
void add_to_executer(Executer *exe, Inst inst){
    exe->program[exe->programLen++] = inst;
}
void executer_load_from_uint8(Executer *executor, uint64_t *content, size_t len){
    size_t i = 0;
    while(i < len){
        Bytecode_Inst_Type opcode = content[i++];
        switch(opcode){
            case BYTECODE_INST_TYPE_PUSH: {
                uint64_t argument = content[i++];
                Inst inst = MAKE_INST(INST_TYPE_PUSH, argument);
                add_to_executer(executor, inst);
            } break;
            case BYTECODE_INST_TYPE_JMP: {
                uint64_t argument = content[i++];
                Inst inst = MAKE_INST(INST_TYPE_JMP, argument);
                add_to_executer(executor, inst);
            } break;
            case BYTECODE_INST_TYPE_DUP: {
                uint64_t argument = content[i++];
                Inst inst = MAKE_INST(INST_TYPE_DUP, argument);
                add_to_executer(executor, inst);
            } break;
            case BYTECODE_INST_TYPE_JMP_IF: {
                uint64_t argument = content[i++];
                Inst inst = MAKE_INST(INST_TYPE_JMP_IF, argument);
                add_to_executer(executor, inst);
            } break;
            case BYTECODE_INST_TYPE_ADDI: {
                Inst inst = MAKE_INST(INST_TYPE_ADDI, 0);
                add_to_executer(executor, inst);
            } break;
            case BYTECODE_INST_TYPE_ADDF: {
                Inst inst = MAKE_INST(INST_TYPE_ADDF, 0);
                add_to_executer(executor, inst);
            } break;
            case BYTECODE_INST_TYPE_MINUSI: {
                Inst inst = MAKE_INST(INST_TYPE_MINUSI, 0);
                add_to_executer(executor, inst);
            } break;
            case BYTECODE_INST_TYPE_MINUSF: {
                Inst inst = MAKE_INST(INST_TYPE_MINUSF, 0);
                add_to_executer(executor, inst);
            } break;
            case BYTECODE_INST_TYPE_EQU: {
                Inst inst = MAKE_INST(INST_TYPE_EQU, 0);
                add_to_executer(executor, inst);
            } break;
            case BYTECODE_INST_TYPE_HALT: {
                Inst inst = MAKE_INST(INST_TYPE_HALT, 0);
                add_to_executer(executor, inst);
            } break;
        }
    }
}
void executer_print_stack(const Executer *exe){
    printf("Stack:\n");
    for(uint64_t i = 0; i < exe->stack_size; ++i){
        printf("    as_u64: %llu as_i64: %lld as_f64: %f as_ptr: 0x%p\n", exe->stack[i].as_u64, exe->stack[i].as_i64, exe->stack[i].as_f64, exe->stack[i].as_ptr);
    }
}
// TODO: Error handling
void execute_inst(Executer *exe, Inst inst){
    switch(inst.type){
        case INST_TYPE_PUSH: {
            exe->stack[exe->stack_size++] = inst.operand;
            exe->ip++;
        } break;
        case INST_TYPE_ADDI: {
            exe->stack[exe->stack_size - 2].as_u64 += exe->stack[exe->stack_size - 1].as_u64;
            exe->stack_size -= 1;
            exe->ip++;
        } break;
        case INST_TYPE_MINUSI: {
            exe->stack[exe->stack_size - 2].as_u64 -= exe->stack[exe->stack_size - 1].as_u64;
            exe->stack_size -= 1;
            exe->ip++;
        } break;
        case INST_TYPE_ADDF: {
            exe->stack[exe->stack_size - 2].as_f64 += exe->stack[exe->stack_size - 1].as_f64;
            exe->stack_size -= 1;
            exe->ip++;
        } break;
        case INST_TYPE_MINUSF: {
            exe->stack[exe->stack_size - 2].as_f64 -= exe->stack[exe->stack_size - 1].as_f64;
            exe->stack_size -= 1;
            exe->ip++;
        } break;
        case INST_TYPE_HALT: {
            exe->halt = 1;
            exe->ip++;
        } break;
        case INST_TYPE_JMP: {
            exe->ip = inst.operand.as_u64 - 1;
        } break;
        case INST_TYPE_JMP_IF: {
            if(exe->stack[exe->stack_size-1].as_u64){
                exe->ip = inst.operand.as_u64 - 1;
            }
            else{
                exe->ip++;
            }
            exe->stack_size -= 1;
        } break;
        case INST_TYPE_EQU: {
            exe->stack[exe->stack_size - 2].as_u64 = exe->stack[exe->stack_size - 1].as_u64 == exe->stack[exe->stack_size - 2].as_u64;
            exe->stack_size -= 1;
            exe->ip++;
        } break;
        case INST_TYPE_DUP: {
            exe->stack[exe->stack_size] = exe->stack[exe->stack_size - 1 - inst.operand.as_u64];
            exe->stack_size += 1;
            exe->ip++;
        } break;
        default: {
            printf("Invalid inst\n");
            exit(1);
        } break;
    }
}
void executer_execute(Executer *exe){
    while(!exe->halt){
        Inst inst = exe->program[exe->ip];
        execute_inst(exe, inst);
    }
    executer_print_stack(exe);
}

int main(){
    size_t len = 0;
    Executer exe = {0};
    uint64_t *content = readFile("main.lexe", &len);
    executer_load_from_uint8(&exe, content, len);
    executer_execute(&exe);
    return 0;
}