#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#define STRING_IMPLEMENTATION
#include <mystring.h>
#include "insts.h"
#include "ast.h"
#define VERSION 0, 0, 1

char *readFile(const char* filePath, size_t *len){
    FILE *f = fopen(filePath, "r");
    if(!f){
        printf("No file or directory named %s\n", filePath);
        return NULL;
    }
    
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *buffer = malloc(size);
    size_t lenRead = 0;
    if((lenRead = fread(buffer, 1, size, f)) == 0){
        printf("Could not read any data\n");
        free(buffer);
        return NULL;
    }
    buffer[lenRead] = '\0';
    *len = lenRead;
    return buffer;
}

int print_ast=0;
char *input_file = NULL;
char *output_file = NULL;

int parse_arguments(int argc, char *argv[]){
    for(int i = 1; i < argc; ++i){
        char *argument = argv[i];

        if(strcmp(argument, "-v") == 0 || strcmp(argument, "--version") == 0){
            printf("lasm version %d.%d.%d\n", VERSION);
            exit(0);
        } else if(strcmp(argument, "-o") == 0 || strcmp(argument, "--output") == 0){
            if(i++ >= argc){
                printf("Expected file after %s\n", argument);
                exit(1);
            }
            output_file = argv[i];
        }
    }
    printf("%s:%s\n", output_file, input_file);
    return 0;
}

// TODO: Command line arguments
int main(int argc, char *argv[]){
    parse_arguments(argc, argv);
    Ast ast = {0};
    size_t fileLen = 0;
    char *fileContent = readFile("main.lasm", &fileLen);
    String content = string_from_cstr(fileContent);
    uint64_t lineNumber = 0;
    while(content.count > 0){
        String line = string_trim(string_chopByDelim(&content, '\n'));
        lineNumber++;
        if(line.count > 0 && *line.data != ';'){
            String opcode = string_trim_left(string_chopByDelim(&line, ' '));
            String argument = string_trim(string_chopByDelim(&line, ';'));
            if(string_eq(opcode, string_from_cstr("push"))){
                String type = string_trim(string_chopByDelim(&argument, ' '));
                if(string_eq(type, string_from_cstr("int")))    addAstInstNode(&ast, INST_TYPE_PUSH, (Operand) {.as_i64 = string_to_int(argument)});
                if(string_eq(type, string_from_cstr("uint")))   addAstInstNode(&ast, INST_TYPE_PUSH, (Operand) {.as_u64 = string_to_int(argument)});
                if(string_eq(type, string_from_cstr("ptr")))    addAstInstNode(&ast, INST_TYPE_PUSH, (Operand) {.as_ptr = (void*)string_to_int(argument)});
                if(string_eq(type, string_from_cstr("float")))  addAstInstNode(&ast, INST_TYPE_PUSH, (Operand) {.as_f64 = strtof(argument.data, NULL)});
            } else if(string_eq(opcode, string_from_cstr("addi"))){
                addAstInstNode(&ast, INST_TYPE_ADDI, (Operand){0});
            } else if(string_eq(opcode, string_from_cstr("addf"))){
                addAstInstNode(&ast, INST_TYPE_ADDF, (Operand){0});
            } else if(string_eq(opcode, string_from_cstr("minusf"))){
                addAstInstNode(&ast, INST_TYPE_MINUSF, (Operand){0});
            } else if(string_eq(opcode, string_from_cstr("equ"))){
                addAstInstNode(&ast, INST_TYPE_EQU, (Operand){0});
            } else if(string_eq(opcode, string_from_cstr("jmp"))){
                uint64_t addr = astFindLabelByName(ast, argument);
                addAstInstNode(&ast, INST_TYPE_JMP, (Operand) {.as_u64 = addr});
            } else if(string_eq(opcode, string_from_cstr("jmp_if"))){
                uint64_t addr = astFindLabelByName(ast, argument);
                addAstInstNode(&ast, INST_TYPE_JMP_IF, (Operand) {.as_u64 = addr});
            } else if(string_eq(opcode, string_from_cstr("dup"))){
                addAstInstNode(&ast, INST_TYPE_DUP, (Operand) {.as_u64 = string_to_int(argument)});
            } else if(string_eq(opcode, string_from_cstr("minusi"))){
                addAstInstNode(&ast, INST_TYPE_MINUSI, (Operand){0});
            } else if(string_eq(opcode, string_from_cstr("halt"))){
                addAstInstNode(&ast, INST_TYPE_HALT, (Operand){0});
            } else if(opcode.data[opcode.count-1] == ':'){
                String name = string_trim(string_chopByDelim(&opcode, ':'));
                addAstLabelNode(&ast, name, lineNumber);
            } else{
                printf("No opcode with name: %.*s\n", (int)opcode.count, opcode.data);
            }
        }
    }
    printAst(ast);
    compileAst(ast, "main.lexe");
    return 0;
}