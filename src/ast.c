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

int astFindStringByName(const Ast *ast, String name){
    int index = 0;
    for(uint64_t i = 0; i < ast->nodes_len; ++i){
        if(ast->nodes[i].type == AST_NODE_TYPE_STRING){
            if(string_eq(ast->nodes[i].as_str.name, name)){
                return index;
            }
            index++;
        }
    }
    printf("No string exists with name: %.*s\n", (int)name.count, name.data);
    exit(1);
}

uint64_t ast_get_strings_len(Ast ast){
    uint64_t index = 0;
    for(uint64_t i = 0; i < ast.nodes_len; ++i){
        if(ast.nodes[i].type == AST_NODE_TYPE_STRING){
            index++;
        }
    }
    return index;
}

Ast ast_lex_content(String content, char *file){
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
                if(string_eq(type, string_from_cstr("int")))         addAstInstNode(&ast, INST_TYPE_PUSH, (Operand) {.as_i64 = string_to_int(argument)});
                else if(string_eq(type, string_from_cstr("uint")))   addAstInstNode(&ast, INST_TYPE_PUSH, (Operand) {.as_u64 = string_to_int(argument)});
                else if(string_eq(type, string_from_cstr("ptr")))    addAstInstNode(&ast, INST_TYPE_PUSH, (Operand) {.as_ptr = (void*)strtoull(argument.data, NULL, 0)});
                else if(string_eq(type, string_from_cstr("float")))  addAstInstNode(&ast, INST_TYPE_PUSH, (Operand) {.as_f64 = strtof(argument.data, NULL)});
                else{                                                printf("%s:%ld ERROR: Invalid push method named `%.*s`\n", file, lineNumber-labelCount, (int) type.count, type.data); exit(1);}
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
                uint64_t strings_len = ast_get_strings_len(ast);
                ast.deferred_operrand[ast.def_opp_size] = (Ast_Deferred_Opperand) {.addr = insts_len+labelCount+strings_len, .label = argument};
                ast.def_opp_size++;
                addAstInstNode(&ast, INST_TYPE_JMP, (Operand) {0});
            } else if(string_eq(opcode, string_from_cstr("jmp_if"))){
                uint64_t insts_len = ast_get_insts_len(ast);
                uint64_t strings_len = ast_get_strings_len(ast);
                ast.deferred_operrand[ast.def_opp_size] = (Ast_Deferred_Opperand) {.addr = insts_len+labelCount+strings_len, .label = argument};
                ast.def_opp_size++;
                addAstInstNode(&ast, INST_TYPE_JMP_IF, (Operand) {0});
            } else if(string_eq(opcode, string_from_cstr("printstr"))){
                int index = astFindStringByName(&ast, argument);
                addAstInstNode(&ast, INST_TYPE_PRINTSTR, (Operand) {.as_u64 = index});
            } else if(string_eq(opcode, string_from_cstr("dup"))){
                addAstInstNode(&ast, INST_TYPE_DUP, (Operand) {.as_u64 = string_to_int(argument)});
            } else if(string_eq(opcode, string_from_cstr("minusi"))){
                addAstInstNode(&ast, INST_TYPE_MINUSI, (Operand){0});
            } else if(string_eq(opcode, string_from_cstr("halt"))){
                addAstInstNode(&ast, INST_TYPE_HALT, (Operand){0});
            } else if(string_eq(opcode, string_from_cstr("drop"))){
                addAstInstNode(&ast, INST_TYPE_DROP, (Operand){0});
            } else if(string_eq(opcode, string_from_cstr("swap"))){
                addAstInstNode(&ast, INST_TYPE_SWAP, (Operand){0});
            } else if(string_eq(opcode, string_from_cstr("read64"))){
                addAstInstNode(&ast, INST_TYPE_READ64, (Operand){0});
            } else if(string_eq(opcode, string_from_cstr("write64"))){
                addAstInstNode(&ast, INST_TYPE_WRITE64, (Operand){0});
            } else if(string_eq(opcode, string_from_cstr("call"))){
                uint64_t insts_len = ast_get_insts_len(ast);
                uint64_t strings_len = ast_get_strings_len(ast);
                ast.deferred_operrand[ast.def_opp_size] = (Ast_Deferred_Opperand) {.addr = insts_len+labelCount+strings_len, .label = argument};
                ast.def_opp_size++;
                addAstInstNode(&ast, INST_TYPE_CALL, (Operand){0});
            } else if(string_eq(opcode, string_from_cstr("ret"))){
                addAstInstNode(&ast, INST_TYPE_RET, (Operand){0});
            } else if(string_eq(opcode, string_from_cstr("inttofloat"))){
                addAstInstNode(&ast, INST_TYPE_INTTOFLOAT, (Operand){0});
            } else if(string_eq(opcode, string_from_cstr("native"))){
                addAstInstNode(&ast, INST_TYPE_NATIVE, (Operand){.as_u64 = string_to_int(argument)});
            } else if(string_eq(opcode, string_from_cstr("str"))){
                String name = string_trim(string_chopByDelim(&argument, '"'));
                String text = string_chopByDelim(&argument, '"');
                addAstStrNode(&ast, name, text);
            } else{
                printf("%s:%ld: ERROR: No opcode with name: %.*s\n", file, lineNumber-labelCount, (int)opcode.count, opcode.data);
                exit(1);
            }
        }
    }
    for (size_t i = 0; i < ast.def_opp_size; ++i) {
        {
            uint64_t addr = ast_find_label_addr(ast, ast.deferred_operrand[i].label);
            ast.nodes[ast.deferred_operrand[i].addr].as_inst.operand.as_u64 = addr;
        }
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

void addAstStrNode(Ast *ast, String name, String data){
    ast->nodes[ast->nodes_len].type = AST_NODE_TYPE_STRING;
    ast->nodes[ast->nodes_len].as_str = make_ast_node_str(name, data);
    ast->nodes_len++;
}

Ast_Node_Str make_ast_node_str(String name, String data){
    Ast_Node_Str label;
    label.name = name;
    label.text = data;
    return label;
}

void printAst(Ast ast){
    printf("AST:\n");
    printf("    {\n");
    for(unsigned int i = 0; i < ast.nodes_len; ++i){
        switch(ast.nodes[i].type){
            case AST_NODE_TYPE_INST: {
                printf("        {inst: {type: %s, operand: 0x%lx}}\n", inst_type_as_cstr(ast.nodes[i].as_inst.type), ast.nodes[i].as_inst.operand.as_u64);
            } break;
            case AST_NODE_TYPE_LABEL: {
                printf("        {label: {name: %.*s, addr: %ld}}\n", (int)ast.nodes[i].as_label.name.count, ast.nodes[i].as_label.name.data, ast.nodes[i].as_label.addr);
            } break;
            case AST_NODE_TYPE_STRING: {
                printf("        {string: {name: %.*s, text: %.*s}}\n", (int)ast.nodes[i].as_str.name.count, ast.nodes[i].as_str.name.data, (int)ast.nodes[i].as_str.text.count, ast.nodes[i].as_str.text.data);
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
                printf("Duplicate name found %.*s is already declared at line %ld\n", (int)name.count, name.data, lineNumber);
            }
        }
    }
    Ast_Node_Label lb = {.addr = addr, .name = name};
    compiler->nodes[compiler->nodes_len].as_label = lb;
    compiler->nodes[compiler->nodes_len].type = AST_NODE_TYPE_LABEL;
    compiler->nodes_len++;
}

void compileUint64_t(uint64_t number, FILE *f){    
    for(uint64_t i = 0; i < sizeof(uint64_t); ++i){
        uint8_t number8 = (uint8_t)((number >> (8 * i)) & 0xFF);
        fwrite(&number8, sizeof(uint8_t), 1, f);
    }
}

uint8_t inst_as_bytecode(Inst_Type type){
    switch(type){
            case INST_TYPE_PUSH: return BYTECODE_INST_TYPE_PUSH;
            case INST_TYPE_ADDI: return BYTECODE_INST_TYPE_ADDI;
            case INST_TYPE_ADDF: return BYTECODE_INST_TYPE_ADDF;
            case INST_TYPE_MINUSI: return BYTECODE_INST_TYPE_MINUSI;
            case INST_TYPE_MINUSF: return BYTECODE_INST_TYPE_MINUSF;
            case INST_TYPE_DUP: return BYTECODE_INST_TYPE_DUP;
            case INST_TYPE_EQU: return BYTECODE_INST_TYPE_EQU;
            case INST_TYPE_JMP: return BYTECODE_INST_TYPE_JMP;
            case INST_TYPE_JMP_IF: return BYTECODE_INST_TYPE_JMP_IF;
            case INST_TYPE_CALL: return BYTECODE_INST_TYPE_CALL;
            case INST_TYPE_RET: return BYTECODE_INST_TYPE_RET;
            case INST_TYPE_DROP: return BYTECODE_INST_TYPE_DROP;
            case INST_TYPE_SWAP: return BYTECODE_INST_TYPE_SWAP;
            case INST_TYPE_NATIVE: return BYTECODE_INST_TYPE_NATIVE;
            case INST_TYPE_PRINTSTR: return BYTECODE_INST_TYPE_PRINTSTR;
            case INST_TYPE_READ64: return BYTECODE_INST_TYPE_READ64;
            case INST_TYPE_WRITE64: return BYTECODE_INST_TYPE_WRITE64;
            case INST_TYPE_INTTOFLOAT: return BYTECODE_INST_TYPE_INTTOFLOAT;
            case INST_TYPE_HALT: return BYTECODE_INST_TYPE_HALT;
            default: return -1;
    }
}

void compileAst(Ast ast, const char *outFile){
    FILE *f = fopen(outFile, "wb");
    uint8_t INST_FINISHED_NUM = 0xfb;
    // fwrite("LASM", 1, 4, f);
    for(unsigned int i = 0; i < ast.nodes_len; ++i){
        switch(ast.nodes[i].type){
            case AST_NODE_TYPE_INST: {
                switch(ast.nodes[i].as_inst.type){
                    case INST_TYPE_CALL: 
                    case INST_TYPE_JMP:
                    case INST_TYPE_PUSH:
                    case INST_TYPE_DUP:
                    case INST_TYPE_NATIVE:
                    case INST_TYPE_PRINTSTR:
                    case INST_TYPE_JMP_IF: {
                        uint8_t bytecodeInst = inst_as_bytecode(ast.nodes[i].as_inst.type);
                        uint64_t bytecodeOperand = ast.nodes[i].as_inst.operand.as_u64;
                        fwrite(&bytecodeInst, sizeof(uint8_t), 1, f);
                        compileUint64_t(bytecodeOperand, f);
                        fwrite(&INST_FINISHED_NUM, sizeof(uint8_t), 1, f);
                    } break;
                    case INST_TYPE_DROP:
                    case INST_TYPE_EQU:
                    case INST_TYPE_ADDF:
                    case INST_TYPE_SWAP:
                    case INST_TYPE_MINUSF:
                    case INST_TYPE_MINUSI: 
                    case INST_TYPE_RET:
                    case INST_TYPE_HALT:
                    case INST_TYPE_INTTOFLOAT:
                    case INST_TYPE_READ64:
                    case INST_TYPE_WRITE64:
                    case INST_TYPE_ADDI: {
                        uint8_t bytecodeInst = inst_as_bytecode(ast.nodes[i].as_inst.type);
                        fwrite(&bytecodeInst, sizeof(uint8_t), 1, f);
                        fwrite(&INST_FINISHED_NUM, sizeof(uint8_t), 1, f);
                    } break;
                    default: {
                        printf("%s: not yet handled in compileAst function\n", inst_type_as_cstr(ast.nodes[i].as_inst.type));
                        exit(1);
                    } break;
                }
            } break;
            case AST_NODE_TYPE_LABEL: {
                
            } break;
            case AST_NODE_TYPE_STRING: {
                int BYTE_AST_NODE_STR = 0xfe;
                fwrite(&BYTE_AST_NODE_STR, sizeof(char), 1, f);
                fwrite(ast.nodes[i].as_str.text.data, sizeof(char), ast.nodes[i].as_str.text.count, f);
                fwrite(&INST_FINISHED_NUM, sizeof(uint8_t), 1, f);
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