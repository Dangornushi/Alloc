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
	BinaryGenerator BinGen;

    vector<Token>  token    = toknizer.tokenize(data);
    vector<Node *> nodes    = parser.parse(token);
	//BinGen.binaryGenerator(nodes);
    vector<string> op_codes = generator.codegen(nodes);

    //vector<string> op_codes = {};

    return op_codes;
}

// binary generator

int main(int argc, char **argv) {
    string   prompt = ">> ";
    ofstream discharge_f;
    FILE    *program_f;
    char    *data;
    char    *fname;
    Mode     mode;

    if (argc == 1) {
        cout << "Alloc interpreter mode." << endl;
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

        program_f = fopen(fname, "r"); // ???????????????????????????????????????NULL????????????
        if (program_f == NULL) {
            printf("%s file not open!\n", fname);
            return -1;
        }

        while (fgets(buf, MAX_READ_SIZE, program_f) != NULL) {
            data = (char *)realloc(data, sizeof(data) + 1);
            strcat(data, buf);
        }

        fclose(program_f); // ????????????????????????
    }

    // file open

    string filename = pick_out_extension(fname) + ".ll";
    discharge_f.open(filename, std::ios::out);

    // file write

    vector<string> op_codes = start(data);

    for (auto item : op_codes) {
        discharge_f << item << std::flush;
    }

    // file close

    discharge_f.close();

    // Null clear

    data = (char *)realloc(NULL, 1);

    // /*===--- Mode ---===*/

    if (mode == INTERP || mode == COMMAND) {
        // run

        int lli_run_return_code = system("lli runtime-file");
        int state               = -1;

        if (WIFEXITED(lli_run_return_code))
            state = WEXITSTATUS(lli_run_return_code);

        cout << "Return code: " << state << endl;
    }
    if (mode == INTERP)
        // go to interprit compile func
        // line: 34
        goto loop;

    free(data);

    return 0;
}
