#pragma once
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "mystring.h"

typedef struct{
    String name;
    uint64_t value;
} PP_Const;

typedef struct{
    PP_Const consts[1024*sizeof(PP_Const)];
    uint64_t consts_count;
} Preprocessor;
typedef Preprocessor PP;

uint64_t pp_get_const(Preprocessor pp, String name);
void pp_add_const(Preprocessor *pp, String name, uint64_t value);