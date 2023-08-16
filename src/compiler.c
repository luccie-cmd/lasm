#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "insts.h"
#include "ast.h"
#define VERSION 0, 1, 0

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
        } else if(strcmp(argument, "-i") == 0 || strcmp(argument, "--input") == 0){
            if(i++ >= argc){
                printf("Expected file after %s\n", argument);
                exit(1);
            }
            input_file = argv[i];
        } else if(strcmp(argument, "--print-ast") == 0){
            print_ast = 1;
        }
    }
    return 0;
}

// TODO: Command line arguments
int main(int argc, char *argv[]){
    parse_arguments(argc, argv);
    Ast ast = {0};
    size_t fileLen = 0;
    char *fileContent = readFile(input_file, &fileLen);
    String content = string_from_cstr(fileContent);
    ast = ast_lex_content(content);
    if(print_ast) printAst(ast);
    compileAst(ast, output_file);
    return 0;
}