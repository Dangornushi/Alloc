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

    vector<string> tokenSet = {
        "fn",
        "__in__",
        "__out__",
        "__put__",
        "return",
        "break",
        "while",
        "<-",
        "if",
        "let",
    };

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
                         bool f = false;
                for (auto t : tokenSet) {
                    int len = t.size();
                    if (split_token(input, t.c_str(), len)) {
                        tokens.push_back(Token{TK_RESERV, t, len});
                        input += len;
                        f = true;
                        break;
                    }
                }
                if (f)
                    continue;

                if  (split_token(input, "//", 2)) {
                    while (*input++ != '\n');
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

                if  (split_token(input, "\"", 1)) {
                    string str_data = "";
                    input++;
                    while (1) {
                        if (*input == '"')
                            break;
                        string char_data = {*input};
                        str_data += char_data;
                        input++;
                    }
                    
                    Token token = {TK_STR, str_data, static_cast<int> (str_data.size())};
                    tokens.push_back(token);
                    input++;
                    continue;
                }

                if (input[0] == '/') {
                    string input_string = input.base();
                    Token  token        = {TK_OP, input_string.substr(0, 1), 1};
                    tokens.push_back(token);
                    input++;
                    continue;
                }

                if (input[0] == '<') {

                    string input_string = input.base();
                    Token  token        = {TK_OP, input_string.substr(0, 1), 1};
                    tokens.push_back(token);
                    input++;
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
