#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <mystring.h>
#include "insts.h"
#include "preprocessor.h"

typedef struct {
    Inst_Type type;
    Operand operand;
} Ast_Node_Inst;

typedef struct {
    String label;
    uint64_t addr;
} Ast_Deferred_Opperand;

typedef struct {
    String name;
    uint64_t addr;
} Ast_Node_Label;

typedef struct {
    String name;
    String text;
} Ast_Node_Str;

typedef enum {
    AST_NODE_TYPE_INST,
    AST_NODE_TYPE_LABEL,
    AST_NODE_TYPE_STRING,
} Ast_Node_Type;

typedef struct{
    Ast_Node_Inst as_inst;
    Ast_Node_Label as_label;
    Ast_Node_Str as_str;
    Ast_Node_Type type;
} Ast_Nodes;

typedef struct{
    Ast_Nodes nodes[1024];
    unsigned int nodes_len;
    Ast_Deferred_Opperand deferred_operrand[1024];
    unsigned int def_opp_size;
} Ast;

uint64_t ast_find_label_addr(Ast ast, String name);
Ast ast_lex_content(String content, char *file);
Ast_Node_Inst make_ast_node_inst(Inst_Type type, Operand operand);
Ast_Node_Label make_ast_node_label(String name, uint64_t line);
Ast_Node_Str make_ast_node_str(String name, String data);
void addAstInstNode(Ast *ast, Inst_Type type, Operand operand);
void addAstLabelNode(Ast *ast, String name, uint64_t line);
void addAstStrNode(Ast *ast, String name, String data);
void printAst(Ast ast);
uint64_t ast_get_insts_len(Ast ast);
uint64_t astFindLabelByName(Ast ast, String name);
void ast_parse_label(Ast *compiler, String name, uint64_t lineNumber);
void compileAst(Ast ast, const char *outFile);
void compileUint64_t(uint64_t number, FILE *f);
uint8_t inst_as_bytecode(Inst_Type type);
int astFindStringByName(const Ast *ast, String name);
uint64_t ast_get_strings_len(Ast ast);