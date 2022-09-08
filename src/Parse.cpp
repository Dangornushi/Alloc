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
};

bool Parser::expect(const char *str) {

    if (consume(str))
        return true;

    cout << "expect: '" << str << "', but get '" << token->str << "'" << endl;
    exit(1);
    return false;
}

Node *Parser::let_exper(void) {
    Node *node;

    node = num();
    expect("<-");
    node = new Node{ND_LET, node, addSub()};
    expect(";");
    return node;
}

Node *Parser::return_exper(void) {
    Node *node = new Node{ND_RETURN, node, addSub()};
    expect(";");
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

Node *Parser::func(void) {
    Node *node;
    if (consume("fn")) {
        string function_type = "void";

        node                 = num();

        if (consume("(")) {
            expect(")");
        }

        if (consume(":")) {
            function_type = token->str;
            token++;
        }

        node = new Node{ND_FN, node, block()};
    }
    return node;
}

Node *Parser::block(void) {
    Node *node;

    expect("{");

    node = expr();

    while (1) {
        if (consume("}"))
            return node;
        node = new Node{ND_BLOCK, node, expr()};
    }

    expect("}");
    return node;
}

Node *Parser::expr(void) {

    if (consume("let")) {
        return let_exper();
    } else if (consume("return")) {
        return return_exper();
    } else {
        return addSub();
    }
}

Node *Parser::addSub(void) {
    Node *node = num();

    while (1) {
        if (consume("+")) {
            node = new Node{ND_ADD, node, addSub()};
        } else if (consume("-"))
            node = new Node{ND_SUB, node, addSub()};
        else if (consume("*"))
            node = new Node{ND_MUL, node, addSub()};
        else if (consume("/"))
            node = new Node{ND_DIV, node, addSub()};
        else
            return node;
    }
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
