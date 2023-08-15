#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <mystring.h>
#include "insts.h"

typedef struct{
    Inst_Type type;
    Operand operand;
} Ast_Node_Inst;

typedef struct{
    String name;
    uint64_t addr;
} Ast_Node_Label;

typedef enum{
    AST_NODE_TYPE_INST,
    AST_NODE_TYPE_LABEL,
} Ast_Node_Type;

typedef struct{
    Ast_Node_Inst as_inst;
    Ast_Node_Label as_label;
    Ast_Node_Type type;
} Ast_Nodes;

typedef struct{
    Ast_Nodes nodes[100];
    unsigned int nodes_len;
} Ast;

Ast_Node_Inst make_ast_node_inst(Inst_Type type, Operand operand){
    Ast_Node_Inst inst;
    inst.operand = operand;
    inst.type = type;
    return inst;
}

Ast_Node_Label make_ast_node_label(String name, uint64_t line){
    Ast_Node_Label label;
    label.addr = line;
    label.name = name;
    return label;
}

void addAstInstNode(Ast *ast, Inst_Type type, Operand operand){
    ast->nodes[ast->nodes_len].type = AST_NODE_TYPE_INST;
    ast->nodes[ast->nodes_len].as_inst = make_ast_node_inst(type, operand);
    ast->nodes_len++;
}

void addAstLabelNode(Ast *ast, String name, uint64_t line){
    ast->nodes[ast->nodes_len].type = AST_NODE_TYPE_LABEL;
    ast->nodes[ast->nodes_len].as_label = make_ast_node_label(name, line);
    ast->nodes_len++;
}

void printAst(Ast ast){
    printf("AST:\n");
    printf("    {\n");
    for(unsigned int i = 0; i < ast.nodes_len; ++i){
        switch(ast.nodes[i].type){
            case AST_NODE_TYPE_INST: {
                printf("        {inst: {type: %s, operand: 0x%llx}}\n", inst_type_as_cstr(ast.nodes[i].as_inst.type), ast.nodes[i].as_inst.operand.as_u64);
            } break;
            case AST_NODE_TYPE_LABEL: {
                printf("        {label: {name: %.*s, addr: %lld}}\n", (int)ast.nodes[i].as_label.name.count, ast.nodes[i].as_label.name.data, ast.nodes[i].as_label.addr);
            } break;
            default: {
                printf("Invalid ast node type with type: %d at node: %d\n", ast.nodes[i].type, i);
                exit(1);
            } break;
        }
    }
    printf("    }\n");
}

uint64_t astFindLabelByName(Ast ast, String name){
    for(unsigned i = 0; i < ast.nodes_len; ++i){
        if(ast.nodes[i].type == AST_NODE_TYPE_LABEL){
            if(string_eq(ast.nodes[i].as_label.name, name)){
                return ast.nodes[i].as_label.addr;
            }
        }
    }
    printf("No label defined with name: %.*s\n", (int)name.count, name.data);
    return -1;
}

void compileAst(Ast ast, const char *outFile){
    FILE *f = fopen(outFile, "wb");
    // fwrite("LASM", 1, 4, f);
    for(unsigned int i = 0; i < ast.nodes_len; ++i){
        switch(ast.nodes[i].type){
            case AST_NODE_TYPE_INST: {
                switch(ast.nodes[i].as_inst.type){
                    case INST_TYPE_PUSH: {
                        uint64_t bytecodeInst = BYTECODE_INST_TYPE_PUSH;
                        uint64_t bytecodeOperand = htole64(ast.nodes[i].as_inst.operand.as_u64);
                        fwrite(&bytecodeInst, sizeof(uint64_t), 1, f);
                        fwrite(&bytecodeOperand, sizeof(uint64_t), 1, f);
                    } break;
                    case INST_TYPE_JMP: {
                        uint64_t bytecodeInst = BYTECODE_INST_TYPE_JMP;
                        uint64_t bytecodeOperand = ast.nodes[i].as_inst.operand.as_u64;
                        fwrite(&bytecodeInst, sizeof(uint64_t), 1, f);
                        fwrite(&bytecodeOperand, sizeof(uint64_t), 1, f);
                    } break;
                    case INST_TYPE_JMP_IF: {
                        uint64_t bytecodeInst = BYTECODE_INST_TYPE_JMP_IF;
                        uint64_t bytecodeOperand = ast.nodes[i].as_inst.operand.as_u64;
                        fwrite(&bytecodeInst, sizeof(uint64_t), 1, f);
                        fwrite(&bytecodeOperand, sizeof(uint64_t), 1, f);
                    } break;
                    case INST_TYPE_DUP: {
                        uint64_t bytecodeInst = BYTECODE_INST_TYPE_DUP;
                        uint64_t bytecodeOperand = ast.nodes[i].as_inst.operand.as_u64;
                        fwrite(&bytecodeInst, sizeof(uint64_t), 1, f);
                        fwrite(&bytecodeOperand, sizeof(uint64_t), 1, f);
                    } break;
                    case INST_TYPE_ADDI: {
                        uint64_t bytecodeInst = BYTECODE_INST_TYPE_ADDI;
                        fwrite(&bytecodeInst, sizeof(uint64_t), 1, f);
                    } break;
                    case INST_TYPE_ADDF: {
                        uint64_t bytecodeInst = BYTECODE_INST_TYPE_ADDF;
                        fwrite(&bytecodeInst, sizeof(uint64_t), 1, f);
                    } break;
                    case INST_TYPE_EQU: {
                        uint64_t bytecodeInst = BYTECODE_INST_TYPE_EQU;
                        fwrite(&bytecodeInst, sizeof(uint64_t), 1, f);
                    } break;
                    case INST_TYPE_MINUSI: {
                        uint64_t bytecodeInst = BYTECODE_INST_TYPE_MINUSI;
                        fwrite(&bytecodeInst, sizeof(uint64_t), 1, f);
                    } break;
                    case INST_TYPE_MINUSF: {
                        uint64_t bytecodeInst = BYTECODE_INST_TYPE_MINUSF;
                        fwrite(&bytecodeInst, sizeof(uint64_t), 1, f);
                    } break;
                    case INST_TYPE_HALT: {
                        uint64_t bytecodeInst = BYTECODE_INST_TYPE_HALT;
                        fwrite(&bytecodeInst, sizeof(uint64_t), 1, f);
                    } break;
                    default: {
                        printf("%s: not yet handled in compileAst function\n", inst_type_as_cstr(ast.nodes[i].as_inst.type));
                        exit(1);
                    } break;
                }
            } break;
            case AST_NODE_TYPE_LABEL: {
                
            } break;
            default: {
                printf("Invalid ast node type at node %d with type %d\n", 
                        i, 
                        ast.nodes[i].type);
                exit(1);
            }
        }
    }
}