#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mystring.h>
#include "insts.h"

typedef struct{
    String name;
    String data;
} Exe_Str;

typedef struct{
    Operand stack[1024*sizeof(Operand)];
    uint64_t stack_size;

    Inst program[1024*sizeof(Inst)];
    uint64_t programLen;

    uint64_t callAddr[1024*sizeof(uint64_t)];
    uint64_t callAddr_len;

    String strings[1024*sizeof(String)];
    uint64_t strings_len;

    uint64_t memory[0xffff*sizeof(uint64_t)];

    uint64_t ip;
    int halt;
} Executer;

typedef enum{
    CALL_ID_PRINT_U64=0,
    CALL_ID_PRINT_I64=1,
    CALL_ID_PRINT_PTR=2,
    CALL_ID_PRINT_F64=3,
} Call_Id;

typedef enum{
    ERROR_OK,
    ERROR_STACK_OVERFLOW,
    ERROR_STACK_UNDERFLOW,
} Error;

char *error_as_cstr(Error err){
    switch(err){
        case ERROR_OK:              return "ERROR_OK";
        case ERROR_STACK_OVERFLOW:  return "ERROR_STACK_OVERFLOW";
        case ERROR_STACK_UNDERFLOW: return "ERROR_STACK_UNDERFLOW";
    }
    return NULL;
}

uint8_t *readFile(const char *filePath, size_t *len){
    FILE *f = fopen(filePath, "rb");
    if(!f){
        printf("No file or directory named %s\n", filePath);
        return NULL;
    }
    
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);

    uint8_t *buffer = malloc(size);
    if(buffer == NULL){
        printf("Failed to allocate memory for buffer\n");
        exit(1);
    }
    size_t lenRead = 0;
    if((lenRead = fread(buffer, 1, size, f)) == 0){
        printf("Could not read any data\n");
        free(buffer);
        return NULL;
    }
    *len = lenRead;
    return buffer;
}

void add_to_executer(Executer *exe, Inst inst){
    exe->program[exe->programLen++] = inst;
}

uint64_t reverse_bytes(uint64_t bytes){
    uint64_t reversedResult = 0;
    while (bytes != 0) {
        reversedResult = (reversedResult << 8) | (bytes & 0xFF);
        bytes >>= 8;
    }
    return reversedResult;
}

uint64_t content_get_operand(uint8_t *content, size_t i){
    uint64_t result = 0;
    while(content[i] != 0xfb){
        uint8_t buffer = content[i++];
        result = (result << 8) | buffer;
    }
    return reverse_bytes(result);
}

void executer_load_from_uint8(Executer *executor, uint8_t *content, size_t len){
    size_t i = 0;
    while(i < len){
        Bytecode_Inst_Type opcode = content[i++];
        switch(opcode){
            case BYTECODE_INST_TYPE_PUSH: {
                uint64_t argument = content_get_operand(content, i);
                Inst inst = MAKE_INST(INST_TYPE_PUSH, argument);
                add_to_executer(executor, inst);
                i+=1+8;
            } break;
            case BYTECODE_INST_TYPE_JMP: {
                uint64_t argument = content_get_operand(content, i);
                Inst inst = MAKE_INST(INST_TYPE_JMP, argument);
                add_to_executer(executor, inst);
                i+=1+8;
            } break;
            case BYTECODE_INST_TYPE_DUP: {
                uint64_t argument = content_get_operand(content, i);
                Inst inst = MAKE_INST(INST_TYPE_DUP, argument);
                add_to_executer(executor, inst);
                i+=1+8;
            } break;
            case BYTECODE_INST_TYPE_CALL: {
                uint64_t argument = content_get_operand(content, i);
                Inst inst = MAKE_INST(INST_TYPE_CALL, argument);
                add_to_executer(executor, inst);
                i+=1+8;
            } break;
            case BYTECODE_INST_TYPE_NATIVE: {
                uint64_t argument = content_get_operand(content, i);
                Inst inst = MAKE_INST(INST_TYPE_NATIVE, argument);
                add_to_executer(executor, inst);
                i+=1+8;
            } break;
            case BYTECODE_INST_TYPE_JMP_IF: {
                uint64_t argument = content_get_operand(content, i);
                Inst inst = MAKE_INST(INST_TYPE_JMP_IF, argument);
                add_to_executer(executor, inst);
                i+=1+8;
            } break;
            case BYTECODE_INST_TYPE_ADDI: {
                Inst inst = MAKE_INST(INST_TYPE_ADDI, 0);
                add_to_executer(executor, inst);
                i+=1;
            } break;
            case BYTECODE_INST_TYPE_SWAP: {
                Inst inst = MAKE_INST(INST_TYPE_SWAP, 0);
                add_to_executer(executor, inst);
                i+=1;
            } break;
            case BYTECODE_INST_TYPE_DROP: {
                Inst inst = MAKE_INST(INST_TYPE_DROP, 0);
                add_to_executer(executor, inst);
                i+=1;
            } break;
            case BYTECODE_INST_TYPE_LEI: {
                Inst inst = MAKE_INST(INST_TYPE_LEI, 0);
                add_to_executer(executor, inst);
                i+=1;
            } break;
            case BYTECODE_INST_TYPE_WRITE64: {
                Inst inst = MAKE_INST(INST_TYPE_WRITE64, 0);
                add_to_executer(executor, inst);
                i+=1;
            } break;
            case BYTECODE_INST_TYPE_READ64: {
                Inst inst = MAKE_INST(INST_TYPE_READ64, 0);
                add_to_executer(executor, inst);
                i+=1;
            } break;
            case BYTECODE_INST_TYPE_INTTOFLOAT: {
                Inst inst = MAKE_INST(INST_TYPE_INTTOFLOAT, 0);
                add_to_executer(executor, inst);
                i+=1;
            } break;
            case BYTECODE_INST_TYPE_ADDF: {
                Inst inst = MAKE_INST(INST_TYPE_ADDF, 0);
                add_to_executer(executor, inst);
                i+=1;
            } break;
            case BYTECODE_INST_TYPE_RET: {
                Inst inst = MAKE_INST(INST_TYPE_RET, 0);
                add_to_executer(executor, inst);
                i+=1;
            } break;
            case BYTECODE_INST_TYPE_MINUSI: {
                Inst inst = MAKE_INST(INST_TYPE_MINUSI, 0);
                add_to_executer(executor, inst);
                i+=1;
            } break;
            case BYTECODE_INST_TYPE_MINUSF: {
                Inst inst = MAKE_INST(INST_TYPE_MINUSF, 0);
                add_to_executer(executor, inst);
                i+=1;
            } break;
            case BYTECODE_INST_TYPE_EQU: {
                Inst inst = MAKE_INST(INST_TYPE_EQU, 0);
                add_to_executer(executor, inst);
                i+=1;
            } break;
            case BYTECODE_INST_TYPE_HALT: {
                Inst inst = MAKE_INST(INST_TYPE_HALT, 0);
                add_to_executer(executor, inst);
                i+=1;
            } break;
            case BYTECODE_INST_TYPE_PRINTSTR: {
                uint64_t argument = content_get_operand(content, i);
                Inst inst = MAKE_INST(INST_TYPE_PRINTSTR, argument);
                add_to_executer(executor, inst);
                i+=1+8;
            } break;
            case BYTECODE_STR_ID: {
                String data;
                char resultD[1024];
                size_t resultiD = 0;
                while(content[i] != 0xfb){
                    uint8_t buffer = content[i++];
                    resultD[resultiD++] = (char)buffer;
                }
                data.count = resultiD;
                data.data = resultD;
                String str = data;
                executor->strings[executor->strings_len++] = str;
                i++;
            } break;
            default: {
                printf("No instrucution with opcode: %d\n", opcode);
                exit(1);
            } break;
        }
    }
}
void executer_print_stack(const Executer *exe){
    printf("Stack:\n");
    for(uint64_t i = 0; i < exe->stack_size; ++i){
        printf("    as_u64: %lu as_i64: %ld as_f64: %f as_ptr: 0x%p\n", exe->stack[i].as_u64, exe->stack[i].as_i64, exe->stack[i].as_f64, exe->stack[i].as_ptr);
    }
}

void execute_call_id(Executer *exe, Call_Id call_id){
    switch(call_id){
        case CALL_ID_PRINT_U64: {
            printf("%ld\n", exe->stack[exe->stack_size-1].as_u64);
        } break;
        case CALL_ID_PRINT_I64: {
            printf("%ld\n", exe->stack[exe->stack_size-1].as_i64);
        } break;
        case CALL_ID_PRINT_PTR: {
            printf("%p\n", exe->stack[exe->stack_size-1].as_ptr);
        } break;
        case CALL_ID_PRINT_F64: {
            printf("%f\n", exe->stack[exe->stack_size-1].as_f64);
        } break;
        default: {
            printf("Invalid call address with number: %d\n", call_id);
            exit(1);
        } break;
    }
}

// TODO: Error handling
Error execute_inst(Executer *exe, Inst inst){
    switch(inst.type){
        case INST_TYPE_PUSH: {
            if(exe->stack_size >= 1024){
                return ERROR_STACK_OVERFLOW;
            }
            exe->stack[exe->stack_size++] = inst.operand;
            exe->ip++;
        } break;
        case INST_TYPE_ADDI: {
            if(exe->stack_size < 2){
                return ERROR_STACK_UNDERFLOW;
            }
            exe->stack[exe->stack_size - 2].as_u64 += exe->stack[exe->stack_size - 1].as_u64;
            exe->stack_size -= 1;
            exe->ip++;
        } break;
        case INST_TYPE_MINUSI: {
            if(exe->stack_size < 2){
                return ERROR_STACK_UNDERFLOW;
            }
            exe->stack[exe->stack_size - 2].as_u64 -= exe->stack[exe->stack_size - 1].as_u64;
            exe->stack_size -= 1;
            exe->ip++;
        } break;
        case INST_TYPE_ADDF: {
            if(exe->stack_size < 2){
                return ERROR_STACK_UNDERFLOW;
            }
            exe->stack[exe->stack_size - 2].as_f64 += exe->stack[exe->stack_size - 1].as_f64;
            exe->stack_size -= 1;
            exe->ip++;
        } break;
        case INST_TYPE_MINUSF: {
            if(exe->stack_size < 2){
                return ERROR_STACK_UNDERFLOW;
            }
            exe->stack[exe->stack_size - 2].as_f64 -= exe->stack[exe->stack_size - 1].as_f64;
            exe->stack_size -= 1;
            exe->ip++;
        } break;
        case INST_TYPE_HALT: {
            exe->halt = 1;
            exe->ip++;
        } break;
        case INST_TYPE_DROP: {
            if(exe->stack_size < 1){
                return ERROR_STACK_UNDERFLOW;
            }
            exe->stack_size -= 1;
            exe->ip++;
        } break;
        case INST_TYPE_SWAP: {
            if(exe->stack_size < 2){
                return ERROR_STACK_UNDERFLOW;
            }
            Operand buffer = exe->stack[exe->stack_size - 2];
            exe->stack[exe->stack_size - 2] = exe->stack[exe->stack_size - 1];
            exe->stack[exe->stack_size - 1] = buffer;
            exe->ip++;
        } break;
        case INST_TYPE_JMP: {
            exe->ip = inst.operand.as_u64 - 1;
        } break;
        case INST_TYPE_JMP_IF: {
            if(exe->stack_size < 1){
                return ERROR_STACK_UNDERFLOW;
            }
            if(exe->stack[exe->stack_size-1].as_u64){
                exe->ip = inst.operand.as_u64 - 1;
            }
            else{
                exe->ip++;
            }
            exe->stack_size -= 1;
        } break;
        case INST_TYPE_EQU: {
            if(exe->stack_size < 2){
                return ERROR_STACK_UNDERFLOW;
            }
            exe->stack[exe->stack_size - 2].as_u64 = exe->stack[exe->stack_size - 1].as_u64 == exe->stack[exe->stack_size - 2].as_u64;
            exe->stack_size -= 1;
            exe->ip++;
        } break;
        case INST_TYPE_LEI: {
            if(exe->stack_size < 2){
                return ERROR_STACK_UNDERFLOW;
            }
            exe->stack[exe->stack_size - 2].as_u64 = exe->stack[exe->stack_size - 1].as_u64 <= exe->stack[exe->stack_size - 2].as_u64;
            exe->stack_size -= 1;
            exe->ip++;
        } break;
        case INST_TYPE_DUP: {
            if(exe->stack_size < 1){
                return ERROR_STACK_UNDERFLOW;
            }
            exe->stack[exe->stack_size] = exe->stack[exe->stack_size - 1 - inst.operand.as_u64];
            exe->stack_size += 1;
            exe->ip++;
        } break;
        case INST_TYPE_CALL: {
            if(exe->callAddr_len >= 1024){
                return ERROR_STACK_OVERFLOW;
            }
            uint64_t jmp_addr = inst.operand.as_u64;
            uint64_t ret_addr = exe->ip+1;
            exe->ip = jmp_addr-1;
            exe->callAddr[exe->callAddr_len++] = ret_addr;
        } break;
        case INST_TYPE_RET: {
            if(exe->callAddr_len < 1){
                return ERROR_STACK_UNDERFLOW;
            }
            uint64_t ret_addr = exe->callAddr[exe->callAddr_len-1];
            exe->callAddr_len -= 1;
            exe->ip = ret_addr;
        } break;
        case INST_TYPE_NATIVE: {
            if(exe->stack_size < 1){
                return ERROR_STACK_UNDERFLOW;
            }
            Call_Id call_id = inst.operand.as_u64;
            execute_call_id(exe, call_id);
            exe->ip++;
        } break;
        case INST_TYPE_PRINTSTR: {
            printf("%.*s\n", (int)exe->strings[inst.operand.as_u64].count, exe->strings[inst.operand.as_u64].data);
            exe->ip++;
        } break;
        case INST_TYPE_INTTOFLOAT: {
            if(exe->stack_size < 2){
                return ERROR_STACK_UNDERFLOW;
            }
            uint64_t integer = exe->stack[exe->stack_size-2].as_u64;
            uint64_t fraction = exe->stack[exe->stack_size-1].as_u64;
            double result = (double)integer + (double)fraction / 100.0;
            exe->stack[exe->stack_size - 2].as_f64 = result;
            exe->stack_size -= 1;
            exe->ip++;
        } break;
        case INST_TYPE_READ64: {
            if(exe->stack_size < 1){
                return ERROR_STACK_UNDERFLOW;
            }
            uint64_t addr = exe->stack[exe->stack_size-1].as_u64;
            exe->stack[exe->stack_size - 1].as_u64 = exe->memory[addr];
            exe->ip++;
        } break;
        case INST_TYPE_WRITE64: {
            if(exe->stack_size < 2){
                return ERROR_STACK_UNDERFLOW;
            }
            uint64_t mem_addr = exe->stack[exe->stack_size - 1].as_u64;
            uint64_t num = exe->stack[exe->stack_size - 2].as_u64;
            exe->memory[mem_addr] = num;
            exe->ip++;
        } break;
        default: {
            printf("Invalid inst with opcode: %d\nip = %ld\n", inst.type, exe->ip);
            exit(1);
        } break;
    }
    return ERROR_OK;
}
void executer_execute(Executer *exe){
    while(!exe->halt){
        Inst inst = exe->program[exe->ip];
        Error err = execute_inst(exe, inst);
        if(err != ERROR_OK){
            printf("ERROR: %s\n", error_as_cstr(err));
            exit(1);
        }
        // executer_print_stack(exe);
    }
}

char *in_file = NULL;
int has_in_file = 0;
int print_stack = 0;

void parse_arguments(int argc, char **argv){
    for(int i = 1; i < argc; ++i){
        char *argument = argv[i];
        if(strcmp(argument, "-v") == 0 || strcmp(argument, "--version") == 0){
            printf("lexe version: %d:%d:%d\n", VERSION);
            exit(0);
        }
        else{
            if(has_in_file){
                printf("ERROR: Can only lex 1 file\n");
                exit(1);
            }
            has_in_file = 1;
            in_file = argument;
        }
    }
}

int main(int argc, char **argv){
    parse_arguments(argc, argv);
    size_t len = 0;
    Executer exe = {0};
    uint8_t *content = readFile(in_file, &len);
    executer_load_from_uint8(&exe, content, len);
    executer_execute(&exe);
    return 0;
}