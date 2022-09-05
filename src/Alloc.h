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

#define MAX_READ_SIZE 1024

using std::cerr;
using std::cout;
using std::endl;
using std::ifstream;
using std::map;
using std::ofstream;
using std::string;
using std::to_string;
using std::vector;

typedef enum {
    TK_NUM,
    TK_OP,
    TK_LET,
    TK_RETURN,
    TK_NEWLINE,
    TK_EQ,
    TK_VARIABLE,
    TK_EOF,
} TokenKind;

typedef struct {
    TokenKind kind;
    string    str;
    int       len;
} Token;

typedef enum {
    ND_NUM,
    ND_ADD,
    ND_SUB,
    ND_MUL,
    ND_DIV,
    ND_LET,
    ND_RETURN,
    ND_VARIABLE,
    ND_EXPR,
} NodeKind;

typedef struct Node Node;

struct Node {
    NodeKind kind;
    Node    *left;
    Node    *right;
    string   val;
};

class Tokenizer {
    char         *cursor;
    bool          isKeyword(char *input);
    vector<Token> tokens;

  public:
    vector<Token> tokenize(string sent);
};

class Parser {
    bool isNumber(string str);
    bool expect(const char *str);

  public:
    bool                    consume(const char *str);
    Node                   *let_exper(void);
    Node                   *return_exper(void);
    vector<Node *>          parse(vector<Token> tokens);
    vector<Node *>          program(vector<Node *> nodes);
    Node                   *func(void);
    Node                   *expr(void);
    Node                   *addSub(void);
    Node                   *num(void);
    vector<Token>::iterator token;
};

typedef enum {
    OP_IR_ADD,
    OP_IR_SUB,
    OP_IR_MUL,
    OP_IR_DIV,
} OP_CODE;

typedef struct {
    string Name;
    string Type;
    string Size;
} Register;

typedef struct {
    Register register_pointer;
    Register register_nonpointer_name;
    string   register_string_name;
    int      type;
} IR;

class Generator {
    int                 register_number;
    map<string, IR>     ir;
    map<string, string> ir_to_name;
    Register            env_register;
    vector<string>      op_codes;
    string              IR_load(string load_register, string loaded_register, string type_1, string type_2, string size);

  public:
    string         gen(const Node *node);
    vector<string> codegen(vector<Node *> &nodes);
    string         generate_regisuter_name(void);
    string         trance_type(string reg);
};
