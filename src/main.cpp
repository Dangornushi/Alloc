#include "Alloc.h"
string pick_out_extension(char *data) {
    string extension;

    while (*data != '.')
        extension += *data++;

    return extension;
}

vector<string> start(char *data) {
    Tokenizer      toknizer;
    Parser         parser;
    Generator      generator;

    vector<Token>  token    = toknizer.tokenize(data);

    vector<Node *> nodes    = parser.parse(token);

    vector<string> op_codes = generator.codegen(nodes);

    return op_codes;
}

int main(int argc, char **argv) {
    string   prompt = ">> ";
    ofstream discharge_f;
    FILE    *program_f;
    char    *data;
    char    *fname;
    Mode     mode;

    if (argc == 1) {
    loop:
        string input_data;

        mode = INTERP;
        data = (char *)malloc(sizeof(char *));

        while (1) {
            string tmp;

            cout << prompt << flush;

            std::getline(cin, tmp);

            input_data += tmp;

            if (tmp == "") {
                data  = const_cast<char *>(input_data.c_str());
                fname = (char *)"runtime-file";
                break;
            }

            if (input_data.back() == '{')
                prompt = ".. ";

            if (input_data.back() == '}')
                prompt = ">> ";

            if (input_data == "q") {
                cout << "exit: 0" << endl;
                exit(0);
            }
        }
    }

    else if (!strcmp(argv[1], "-run")) {
        data                     = (char *)malloc(1);
        char *flag_pointer_alloc = (char *)realloc(data, strlen(argv[2]));
        mode                     = COMMAND;

        if (flag_pointer_alloc == NULL) {
            cout << "executinon Err: Cant allocated mem. (-1)" << endl;
            exit(-1);
        }

        strcpy(data, argv[2]);
        fname = (char *)"runtime-file";
    }

    else {
        mode  = COMPILE;
        fname = argv[1];
        data  = (char *)malloc(sizeof(char *));
        char buf[MAX_READ_SIZE];

        program_f = fopen(fname, "r"); // ファイルを開く。失敗するとNULLを返す。
        if (program_f == NULL) {
            printf("%s file not open!\n", fname);
            return -1;
        }

        while (fgets(buf, MAX_READ_SIZE, program_f) != NULL) {
            data = (char *)realloc(data, sizeof(data) + 1);
            strcat(data, buf);
        }

        fclose(program_f); // ファイルを閉じる
    }

    discharge_f.open(pick_out_extension(fname) + ".ll", std::ios::out);

    vector<string> op_codes = start(data);

    for (auto item : op_codes) {
        discharge_f << item << std::flush;
    }

    discharge_f.close();

    data = (char *)realloc(NULL, 1);

    if (mode == INTERP || mode == COMMAND) {
        // run
        int lli_run_return_code = system("lli runtime-file");
        int state               = -1;

        if (WIFEXITED(lli_run_return_code))
            state = WEXITSTATUS(lli_run_return_code);

        cout << "Return code: " << state << endl;
    }
    if (mode == INTERP) {
        // go to interprit compile func
        goto loop;
    }

    free(data);

    return 0;
}
