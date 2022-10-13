#include "Alloc.h"

bool Parser::consume(const char *str) {
    if (TK_EOF == token->kind)
        return false;

    string tmp = str;

    if (tmp == token->str && TK_EOF == token->kind)
        return true;

    if (tmp == token->str) {
        token++;
        return true;
    }

    return false;
}

bool Parser::expect(const char *str) {

    if (consume(str))
        return true;

    cout << "expect: '" << str << "', but get '" << token->str << "'" << endl;
    exit(1);
    return false;
}

Node *Parser::let_exper(void) {
    Node  *node;

    string variable_name = token->str;
    string variable_type;

    token++;

    if (consume(":")) {
        variable_type = token->str;
        token++;
    }

    expect("<-");

    node = new Node{ND_LET, node, boolen(), variable_name, variable_type};

    expect(";");
    return node;
}

Node *Parser::mov_exper(void) {
    Node  *node;

    string variable_name = token->str;
    string variable_type;
    token++;
    expect("<-");
    node = new Node{ND_MOV, node, add_sub(), variable_name};
    expect(";");
    return node;
}

Node *Parser::return_exper(void) {
    Node *node = new Node{ND_RETURN, node, add_sub()};
    expect(";");
    return node;
}

Node *Parser::if_exper(void) {
    /* TODO bool -> add_sub 入れ替え */
    /* そのほか　変数定義するとことかも変える*/
    Node *node = new Node{ND_IF, boolen(), block()};
    return node;
}

Node *Parser::while_exper(void) {
    Node *node = new Node{ND_WHILE, boolen(), block()};
    return node;
}

Node *Parser::__put_exper(void) {
    Node *node = new Node{ND___PUT__, node,  add_sub()};
    consume(";");
    return node;
}

vector<Node *> Parser::parse(vector<Token> tokens) {
    token = tokens.begin();
    vector<Node *> nodes;
    nodes = program(nodes);
    return nodes;
}

vector<Node *> Parser::program(vector<Node *> nodes) {

    while (TK_EOF != token->kind) {
        nodes.push_back(func());
    }
    return nodes;
}

vector<Register> Parser::arg(void) {
    Node            *node;
    vector<Register> argments;

    if (consume(")")) {
        token--;
        return argments;
    }

    while (1) {
        if ((token + 1)->str == ":") {
            argments.push_back({token->str, (token + 2)->str});
            if ((token + 3)->str == ",")
                token += 4;
            else if ((token + 3)->str == ")") {
                token += 3;
                break;
            }
        } else {
            argments.push_back({token->str});
            if ((token + 1)->str == ",")
                token += 2;
            else if ((token + 1)->str == ")") {
                token += 1;
                break;
            }
        }
    }

    return argments;
}

Node *Parser::func(void) {
    Node *node;
    if (consume("fn")) {
        string           function_type = "void";
        string           function_name = token->str;
        vector<Register> argments;
        token++;

        if (consume("(")) {
            argments = arg();
            // argments
            expect(")");
        }

        if (consume(":")) {
            // functions type
            function_type = token->str;
            token++;
        }

        return new Node{ND_FN, node, block(), function_name, function_type, argments};        
    }
    return node;
}

Node *Parser::block(void) {
    Node *node;

    if (token->str != "{")
        return new Node{ND_BLOCK, node, expr()};

    expect("{");

    node = expr();

    while (1) {
        if (consume("}") || token->kind == TK_EOF ) {
            return node;
        }
        node = new Node{ND_BLOCK, node, expr()};
    }

    expect("}");
    return node;
}

Node *Parser::expr(void) {
    if (consume("let"))
        return let_exper();
    else if (consume("return"))
        return return_exper();
    else if (consume("if")) 
        return if_exper();
    else if (consume("while"))
        return while_exper();
    else if (consume("__put__"))
        return __put_exper();
    else if (consume("break")) {
        Node *node = new Node{ND_BREAK, node, expr()};
        return node;
    }
    else if ((token + 1)->str == "<-")
        return mov_exper();
    
    return add_sub();
}

Node *Parser::boolen(void) {
    Node *node = add_sub();

    if (consume(">"))
        return new Node {ND_GREATER_THAN, node, add_sub()};
    if (consume("<"))
        return new Node {ND_LESS_THAN, node, add_sub()};
    return node;
}

Node *Parser::add_sub(void) {
    Node *node = mul_div();

    while (1) {
        if (consume("+")) {
            node = new Node{ND_ADD, node, add_sub()};
        } else if (consume("-")) {
            node = new Node{ND_SUB, node, add_sub()};
        }
        else
            return node;
    }
}

Node *Parser::mul_div(void) {
    Node *node = expr_in_brackets();

    while (1) {
        if (consume("*"))
            node = new Node{ND_MUL, node, mul_div()};
        else if (consume("/"))
            node = new Node{ND_DIV, node, mul_div()};
        else
            return node;
    }
}

Node *Parser::expr_in_brackets(void) {
    if (consume("(")) {
        Node *node = add_sub();
        expect(")");
        return node;
    } else if (TK_VARIABLE == token->kind && (token + 1)->str == "(") {
        return call_function();
    }
    return borrow();
}

Node *Parser::call_arg(void) {
    if (token->str == ")")
        return new Node{};

    Node *node = new Node {ND_CALL_ARG, add_sub()};

    if (consume(","))
        node->right = call_arg();
    return node;
}

Node *Parser::call_function(void) {
    Node *node = new Node();
    node->kind = ND_CALL_FUNCTION;
    node->val  = token->str;
    token++;
    expect("(");
    node->right = call_arg();
    expect(")");
    return node;
}

Node *Parser::borrow(void) {
    if (consume("&")) {
        Node *node = new Node{ND_BORROW_VAR};
        node->val  = token->str;
        token++;
        return node;
    }
    return increment();
}

Node *Parser::increment(void) {
    if ((token+1)->str == "+" && (token+2)->str == "+") {
        string var_name = token->str;
        Node *var = num();
        token += 2;
        Node *one = new Node {ND_NUM};
        one->val = "1";
        return new Node{ND_MOV, var, new Node{ND_ADD, var, one}, var_name};
    }
    if ((token+1)->str == "-" && (token+2)->str == "-") {
        string var_name = token->str;
        Node *var = num();
        token += 2;
        Node *one = new Node {ND_NUM};
        one->val = "1";
        return new Node{ND_MOV, var, new Node{ND_SUB, var, one}, var_name};
    }
    return num();
}

Node *Parser::num(void) {
    Node *node = new Node();
    node->val  = token->str;

    if (isNumber(node->val))
        node->kind = ND_NUM;
    else
        node->kind = ND_VARIABLE;

    token++;
    return node;
}

