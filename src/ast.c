#define STRING_IMPLEMENTATION
#include "ast.h"

uint64_t ast_find_label_addr(Ast ast, String name){
    for(unsigned int i = 0; i < ast.nodes_len; ++i){
        if(ast.nodes[i].type == AST_NODE_TYPE_LABEL){
            if(string_eq(ast.nodes[i].as_label.name, name)){
                return ast.nodes[i].as_label.addr;
            }
        }
    }
    printf("No label with name: %.*s\n", (int)name.count, name.data);
    exit(1);
}

Ast ast_lex_content(String content){
    Ast ast = {0};
    uint64_t lineNumber = 0;
    unsigned int labelCount = 0;
    while(content.count > 0){
        String line = string_trim(string_chopByDelim(&content, '\n'));
        lineNumber++;
        String opcode = string_trim_left(string_chopByDelim(&line, ' '));
        String argument = string_trim(string_chopByDelim(&line, ';'));
        if(opcode.data[opcode.count - 1] == ':'){
            String label_name = string_chopByDelim(&opcode, ':');
            ast_parse_label(&ast, label_name, ast_get_insts_len(ast)+1);
            lineNumber++;
            labelCount++;
            opcode = string_trim(string_chopByDelim(&line, ' '));
        }
        if(opcode.count > 0 && *opcode.data != ';'){
            if(string_eq(opcode, string_from_cstr("push"))){
                String type = string_trim(string_chopByDelim(&argument, ' '));
                if(string_eq(type, string_from_cstr("int")))    addAstInstNode(&ast, INST_TYPE_PUSH, (Operand) {.as_i64 = string_to_int(argument)});
                else if(string_eq(type, string_from_cstr("uint")))   addAstInstNode(&ast, INST_TYPE_PUSH, (Operand) {.as_u64 = string_to_int(argument)});
                else if(string_eq(type, string_from_cstr("ptr")))    addAstInstNode(&ast, INST_TYPE_PUSH, (Operand) {.as_ptr = (void*)strtoull(argument.data, NULL, 0)});
                else if(string_eq(type, string_from_cstr("float")))  addAstInstNode(&ast, INST_TYPE_PUSH, (Operand) {.as_f64 = strtof(argument.data, NULL)});
            } else if(string_eq(opcode, string_from_cstr("addi"))){
                addAstInstNode(&ast, INST_TYPE_ADDI, (Operand){0});
            } else if(string_eq(opcode, string_from_cstr("addf"))){
                addAstInstNode(&ast, INST_TYPE_ADDF, (Operand){0});
            } else if(string_eq(opcode, string_from_cstr("minusf"))){
                addAstInstNode(&ast, INST_TYPE_MINUSF, (Operand){0});
            } else if(string_eq(opcode, string_from_cstr("equ"))){
                addAstInstNode(&ast, INST_TYPE_EQU, (Operand){0});
            } else if(string_eq(opcode, string_from_cstr("jmp"))){
                uint64_t insts_len = ast_get_insts_len(ast);
                ast.deferred_operrand[ast.def_opp_size] = (Ast_Deferred_Opperand) {.addr = insts_len+labelCount, .label = argument};
                ast.def_opp_size++;
                addAstInstNode(&ast, INST_TYPE_JMP, (Operand) {0});
            } else if(string_eq(opcode, string_from_cstr("jmp_if"))){
                uint64_t insts_len = ast_get_insts_len(ast);
                ast.deferred_operrand[ast.def_opp_size] = (Ast_Deferred_Opperand) {.addr = insts_len+labelCount, .label = argument};
                ast.def_opp_size++;
                addAstInstNode(&ast, INST_TYPE_JMP_IF, (Operand) {0});
            } else if(string_eq(opcode, string_from_cstr("dup"))){
                addAstInstNode(&ast, INST_TYPE_DUP, (Operand) {.as_u64 = string_to_int(argument)});
            } else if(string_eq(opcode, string_from_cstr("minusi"))){
                addAstInstNode(&ast, INST_TYPE_MINUSI, (Operand){0});
            } else if(string_eq(opcode, string_from_cstr("halt"))){
                addAstInstNode(&ast, INST_TYPE_HALT, (Operand){0});
            } else{
                printf("No opcode with name: %.*s\n", (int)opcode.count, opcode.data);
                exit(1);
            }
        }
    }
    for (size_t i = 0; i < ast.def_opp_size; ++i) {
        uint64_t addr = ast_find_label_addr(ast, ast.deferred_operrand[i].label);
        ast.nodes[ast.deferred_operrand[i].addr].as_inst.operand.as_u64 = addr;
    }
    return ast;
}

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

uint64_t ast_get_insts_len(Ast ast){
    uint64_t result = 0;
    for(unsigned int i = 0; i < ast.nodes_len; ++i){
        if(ast.nodes[i].type == AST_NODE_TYPE_INST){
            result++;
        }
    }
    return result;
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
    return 0;
}

void ast_parse_label(Ast *compiler, String name, uint64_t lineNumber){
    uint64_t addr = lineNumber;
    name = string_trim(name);
    for(unsigned int i = 0; i < compiler->nodes_len; ++i){
        if(compiler->nodes[i].type == AST_NODE_TYPE_LABEL){
            if(string_eq(name, compiler->nodes[i].as_label.name)){
                printf("Duplicate name found %.*s is already declared at line %lld\n", (int)name.count, name.data, lineNumber);
            }
        }
    }
    Ast_Node_Label lb = {.addr = addr, .name = name};
    compiler->nodes[compiler->nodes_len].as_label = lb;
    compiler->nodes[compiler->nodes_len].type = AST_NODE_TYPE_LABEL;
    compiler->nodes_len++;
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
                        uint64_t bytecodeOperand = ast.nodes[i].as_inst.operand.as_u64;
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
                        printf("%lld\n", ast.nodes[i].as_inst.operand.as_u64);
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