#pragma once
#include <stdint.h>

typedef struct{
    char* data;
    unsigned long long count;
} String;

String string_from_cstr(char* cstr);
String string_slurp_file(const char* file_path);
String string_chopByDelim(String *str, char delim);
String string_trim_left(String str);
String string_trim_right(String str);
String string_trim(String str);
int string_eq(String str1, String str2);
uint64_t string_to_int(String str);

#ifdef STRING_IMPLEMENTATION
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>

String string_from_cstr(char* cstr){
    return (String) {.data = cstr, .count = strlen(cstr)};
}
String string_slurp_file(const char* file_path){
    FILE *f = fopen(file_path, "r");
    if (f == NULL) {
        fprintf(stderr, "ERROR: Could not read file `%s`: %s\n",
                file_path, strerror(errno));
        exit(1);
    }

    if (fseek(f, 0, SEEK_END) < 0) {
        fprintf(stderr, "ERROR: Could not read file `%s`: %s\n",
                file_path, strerror(errno));
        exit(1);
    }

    long m = ftell(f);
    if (m < 0) {
        fprintf(stderr, "ERROR: Could not read file `%s`: %s\n",
                file_path, strerror(errno));
        exit(1);
    }

    char *buffer = malloc(m);
    if (buffer == NULL) {
        fprintf(stderr, "ERROR: Could not allocate memory for file: %s\n",
                strerror(errno));
        exit(1);
    }

    if (fseek(f, 0, SEEK_SET) < 0) {
        fprintf(stderr, "ERROR: Could not read file `%s`: %s\n",
                file_path, strerror(errno));
        exit(1);
    }

    size_t n = fread(buffer, 1, m, f);
    if (ferror(f)) {
        fprintf(stderr, "ERROR: Could not read file `%s`: %s\n",
                file_path, strerror(errno));
        exit(1);
    }

    fclose(f);

    return (String) {
        .count = n,
        .data = buffer,
    };
}
String string_chopByDelim(String *str, char delim){
    size_t i = 0;
    while (i < str->count && str->data[i] != delim && str->data[i] != '\n') {
        i += 1;
    }

    String result = {
        .count = i,
        .data = str->data,
    };

    if (i < str->count) {
        str->count -= i + 1;
        str->data  += i + 1;
    } else {
        str->count -= i;
        str->data  += i;
    }

    return result;
}
String string_trim_left(String str)
{
    size_t i = 0;
    while (i < str.count && isspace(str.data[i])) {
        i += 1;
    }

    return (String) {
        .count = str.count - i,
        .data = str.data + i,
    };
}

String string_trim_right(String str)
{
    size_t i = 0;
    while (i < str.count && isspace(str.data[str.count - 1 - i])) {
        i += 1;
    }

    return (String) {
        .count = str.count - i,
        .data = str.data
    };
}

String string_trim(String str)
{
    return string_trim_right(string_trim_left(str));
}
int string_eq(String str1, String str2){
    if(str1.count != str2.count){
        return 0;
    }
    return memcmp(str1.data, str2.data, str1.count) == 0;
}
uint64_t string_to_int(String str){
    int result = 0;

    for (size_t i = 0; i < str.count && isdigit(str.data[i]); ++i) {
        result = result * 10 + (int) str.data[i] - '0';
    }

    return result;
}

#endif // STRING_IMPLEMENTATION