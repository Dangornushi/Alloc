#include "Alloc.h"

char *variable_name;

bool  split_token(string::iterator data, const char *word, int index) {
     for (int i = 0; i < index; i++)
        if (data[i] != word[i])
            return false;
    return true;
}

bool isAlphabet(string::iterator input) {
    return ((input[0] >= 'a' && input[0] <= 'z') || (input[0] >= 'A' && input[0] <= 'Z') || (input[0] == '_'));
}

bool Tokenizer::isKeyword(char *input) {

    return false;
    // vector<Token>
}

vector<Token> Tokenizer::tokenize(string sent) {
    auto input = sent.begin();

    for (int _size = 0; _size < sent.length(); _size++ ) {
        switch (*input) {
            case '\n':
            case '\t':
            case ' ':
                input++;
                break;
            case '.':
            case ',':
            case ';':
            case ':':
            case '&':
            case '>':
            case '[':
            case ']':
            case '(':
            case ')':
            case '{':
            case '}':
            case '*':
            case '+':
            case '-': {
                string input_string = input.base();
                Token  token        = {TK_OP, input_string.substr(0, 1), 1};
                tokens.push_back(token);
                input++;
                break;
            }
            default: {
                if (split_token(input, "if", 2)) {
                    tokens.push_back(Token{TK_RESERV, "if", 2});
                    input += 2;
                    continue;
                }

                if (split_token(input, "let", 3)) {
                    tokens.push_back(Token{TK_LET, "let", 3});
                    input += 3;
                    continue;
                }

                if  (split_token(input, "//", 2)) {
                    while (*input != '\n') {
                        input++;
                    }
                    continue;
                }

                if  (split_token(input, "/*", 2)) {
                    input+=2;
                    while (1) {
                        if (*input == '*' && *(input+ 1) == '/')
                            break;
                        input++;
                    }
                    input+=2;
                    continue;
                }


                if (input[0] == '/') {
                    string input_string = input.base();
                    Token  token        = {TK_OP, input_string.substr(0, 1), 1};
                    tokens.push_back(token);
                    input++;
                    continue;
                }

                if (split_token(input, "<-", 2)) {
                    tokens.push_back(Token{TK_LET, "<-", 2});
                    input += 2;
                    continue;
                }

                if (input[0] == '<') {

                    string input_string = input.base();
                    Token  token        = {TK_OP, input_string.substr(0, 1), 1};
                    tokens.push_back(token);
                    input++;
                    continue;
                }

                if (split_token(input, "while", 5)) {
                    tokens.push_back(Token{TK_RESERV, "while", 5});
                    input += 5;
                    continue;
                }

                if (split_token(input, "break", 5)) {
                    tokens.push_back(Token{TK_RESERV, "break", 5});
                    input += 5;
                    continue;
                }

                if (split_token(input, "return", 6)) {
                    tokens.push_back(Token{TK_RETURN, "return", 6});
                    input += 6;
                    continue;
                }

                if (split_token(input, "__put__", 7)) {
                    tokens.push_back(Token{TK_RESERV, "__put__", 7});
                    input += 7;
                    continue;
                }

                if (split_token(input, "fn", 2)) {
                    tokens.push_back(Token{TK_FN, "fn", 2});
                    input += 2;
                    continue;
                }

                if (isAlphabet(input)) {
                    string data = "";
                    while (isAlphabet(input)) {
                        data += *input++;
                    }
                    Token token = {TK_VARIABLE, data, (int)data.size()};
                    tokens.push_back(token);
                    continue;
                }
                if (isdigit(*input)) {
                    string input_string = "";

                    for (; isdigit(*input); input++) {
                        input_string += *input;
                    }
                    tokens.push_back(Token{TK_NUM, input_string, 0});
                    continue;
                }
                break;
            }
        }
    }
    tokens.push_back(Token{TK_EOF, "", 0});
    return tokens;
}
