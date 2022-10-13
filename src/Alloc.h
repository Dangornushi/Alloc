#pragma once

#include <vector>
#include <map>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <cstring>
#include <cstdio>
#include <ctype.h>
#include <fstream>
#include <algorithm>
#include <cctype>

#define MAX_READ_SIZE 1024
#define POINTER 0
#define NOTPOINTER 1
#define REALNUMBER 2

using std::cerr;
using std::cin;
using std::cout;
using std::endl;
using std::flush;
using std::ifstream;
using std::map;
using std::ofstream;
using std::string;
using std::to_string;
using std::vector;

typedef enum {
    /* ./alloc '$FILE_DATA' */
    COMMAND,

    /* ./alloc
     * >> $FILE_DATA
     */
    INTERP,

    /* ./alloc $FILE */
    COMPILE,
} Mode;

typedef enum {
    TK_NUM,
    TK_OP,
    TK_LET,
    TK_RETURN,
    TK_FN,
    TK_NEWLINE,
    TK_EQ,
    TK_VARIABLE,
    TK_EOF,
    TK_RESERV,
} TokenKind;

typedef struct {
    TokenKind kind;
    string    str;
    int       len;
} Token;

typedef struct {
    string Name;
    string Type;
    string Size;
} Register;

typedef enum {
    ND_NUM,                 // 0
    ND_ADD,                 // 1
    ND_SUB,                 // 2
    ND_MUL,                 // 3
    ND_DIV,                 // 4
    ND_LET,                 // 5
    ND_MOV,                 // 6
    ND_FN,                  // 7
    ND_RETURN,              // 8
    ND_BLOCK,               // 9
    ND_CALL_ARG,            // 10
    ND_CALL_FUNCTION,       // 11
    ND_VARIABLE,            // 12
    ND_ARG_VARIABLE,        // 13
    ND_BORROW_VAR,          // 14
    ND_EXPR,                // 15
    ND_BREAK,
    ND_IF,
    ND_WHILE,
    ND___PUT__,
    ND_LESS_THAN,
    ND_GREATER_THAN,
    ND_ELSE,
    ND_LOOP,
} NodeKind;

typedef struct Node Node;

struct Node {
    NodeKind         kind;
    Node            *left;
    Node            *right;
    string           val;
    string           type;
    vector<Register> argments;
};

class Tokenizer {
    char         *cursor;
    bool          isKeyword(char *input);
    vector<Token> tokens;

  public:
    vector<Token> tokenize(string sent);
};

class Parser {
    bool                    isNumber(string str);
    bool                    expect(const char *str);
    vector<Token>::iterator token;
    vector<Node *>          program(vector<Node *> nodes);
    bool                    consume(const char *str);
    Node                   *let_exper(void);
    Node                   *mov_exper(void);
    Node                   *return_exper(void);
    Node                   *if_exper(void);
    Node                   *while_exper(void);
    Node                   *__put_exper(void);
    vector<Register>        arg(void);
    Node                   *func(void);
    Node                   *block(void);
    Node                   *expr(void);
    Node                   *expr_in_brackets(void);
    Node                   *boolen(void);
    Node                   *add_sub(void);
    Node                   *mul_div(void);
    Node                   *call_arg(void);
    Node                   *call_function(void);
    Node                   *borrow(void);
    Node                   *increment(void);
    Node                   *num(void);

  public:
    vector<Node *> parse(vector<Token> tokens);
};

typedef enum {
    OP_IR_ADD,
    OP_IR_SUB,
    OP_IR_MUL,
    OP_IR_DIV,
} OP_CODE;

typedef struct {
    Register register_pointer;
    Register register_nonpointer_name;
    string   register_string_name;
    int   Type;
} IR;

class Generator {
    int                 register_number;
    int                 string_declaration_number;
    int                 if_frequency;
    int                 while_frequency;
    bool                returned;
    map<string, bool>   defined_function;
    map<string, IR>     ir;
    map<string, string> ir_to_name;
    map<string, string> type_to_i_type;
    Register            env_register;
    vector<string>      op_codes;
    vector<string>      str_defines;
    vector<string>      function_declaration;
    vector<string>      call_function_argments;
    void                IR_mov(const Node *node, vector<string> &op_codes);
    void                IR_if(const Node *node, vector<string> &op_codes);
    void                IR_while(const Node *node, vector<string> &op_codes);
    void                IR___put__(const Node *node);
    string              IR_load(string load_register, string loaded_register, string type_1, string type_2, string size);
    void                IR_boolen(const Node *node, vector<string> &op_codes, const char calc_op, string &result_register);
    void                IR_calculation(const Node *node, vector<string> &op_codes, const char calc_op, string &result_register);
    void                IR_mov_reDefine(const Node *node);
    string              now_function_type;
    string              while_end_register;

  public:
    string         gen(const Node *node);
    vector<string> codegen(vector<Node *> &nodes);
    string         generate_regisuter_name(void);
    string         trance_type(string reg);
};
