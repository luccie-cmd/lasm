#include "preprocessor.h"

uint64_t pp_get_const(Preprocessor pp, String name){
    for(uint64_t i = 0; i < pp.consts_count; ++i){
        if(string_eq(name, pp.consts[i].name)){
            return pp.consts[i].value;
        }
    }
    printf("No const defined named: %.*s\n", (int)name.count, name.data);
    exit(1);
}

void pp_add_const(Preprocessor *pp, String name, uint64_t value){
    if(pp->consts_count >= 1024){
        printf("ERROR: Preprocessor const overflow\n");
        exit(1);
    }
    for(uint64_t i = 0; i < pp->consts_count; ++i){
        if(string_eq(name, pp->consts[i].name)){
            printf("Cant redifine already defined const with name: %.*s\n", (int)name.count, name.data);
            exit(1);
        }
    }
    PP_Const Const = {0};
    Const.name = name;
    Const.value = value;
    pp->consts[pp->consts_count++] = Const;
}
