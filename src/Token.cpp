#include "Alloc.h"

char *variable_name;

bool  split_token(string::iterator data, const char *word, int index) {
     for (int i = 0; i < index; i++)
        if (data[i] != word[i])
            return false;
    return true;
}

bool isAlphabet(string::iterator input) {
    return ((input[0] >= 'a' && input[0] <= 'z') || (input[0] >= 'A' && input[0] <= 'Z'));
}

bool Tokenizer::isKeyword(char *input) {

    return false;
    // vector<Token>
}

vector<Token> Tokenizer::tokenize(string sent) {
    auto input = sent.begin();

    while (1) {

        switch (*input) {
            case 0:
                tokens.push_back(Token{TK_EOF, "", 0});
                return tokens;
            case '\n':
            case '\t':
            case ' ':
                input++;
                break;
            case ';':
            case '(':
            case ')':
            case '{':
            case '}':
            case ':':
            case '/':
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
                if (split_token(input, "let", 3)) {
                    tokens.push_back(Token{TK_LET, "let", 3});
                    input += 3;
                    continue;
                }

                if (split_token(input, "<-", 2)) {
                    tokens.push_back(Token{TK_LET, "<-", 2});
                    input += 2;
                    continue;
                }

                if (split_token(input, "return", 6)) {
                    tokens.push_back(Token{TK_RETURN, "return", 6});
                    input += 6;
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
                cerr << "Invalid character : " << *input << endl;
                exit(1);
                break;
            }
        }
    }
    return tokens;
}
