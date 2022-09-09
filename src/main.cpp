#include "Alloc.h"

string pick_out_extension(char *data) {
    string extension;

    while (*data != '.')
        extension += *data++;

    return extension;
}

int main(int argc, char **argv) {
    Tokenizer toknizer;
    Parser    parser;
    Generator generator;
    ofstream  discharge_f;
    FILE     *program_f;

    char     *fname = argv[1];
    char      buf[MAX_READ_SIZE];
    char     *data = (char *)malloc(sizeof(char *));

    program_f      = fopen(fname, "r"); // ファイルを開く。失敗するとNULLを返す。
    if (program_f == NULL) {
        printf("%s file not open!\n", fname);
        return -1;
    }

    while (fgets(buf, MAX_READ_SIZE, program_f) != NULL) {
        data = (char *)realloc(data, sizeof(data) + 1);
        strcat(data, buf);
    }

    fclose(program_f); // ファイルを閉じる

    vector<Token>  token    = toknizer.tokenize(data);

    vector<Node *> nodes    = parser.parse(token);

    vector<string> op_codes = generator.codegen(nodes);

    discharge_f.open(pick_out_extension(fname) + ".ll", std::ios::out);

    for (auto item : op_codes) {
        discharge_f << item << std::flush;
    }
    discharge_f.close();

    free(data);

    return 0;
}
