#include "Alloc.h"

bool isdigit(string s1) {
    bool f = true;
    for (auto str : s1)
        f = isdigit(str);
    return f;
}

string Generator::generate_regisuter_name(void) {
    return "%" + to_string(register_number++);
}

string Generator::trance_type(string reg) {
    string ret;

    (reg.at(0) == '%') ? ret = ir[ir_to_name[reg]].register_pointer.Type
                       : ret = "i32";

    return ret;
}

string Generator::IR_load(string load_register, string loaded_register, string type_1, string type_2, string size) {
    return "\t" + load_register + " = load " + type_1 + ", " + type_2 + " " + loaded_register + ", align " + size + "\n";
}

void Generator::IR_boolen(const Node *node, vector<string> &op_codes, const char calc_op, string &result_register) {
    string op_code;

    string left_register = gen(node->left);
    string right_register = gen(node->right);

    if (ir[ir_to_name[left_register]].Type != REALNUMBER && ir[ir_to_name[right_register]].Type != REALNUMBER) {
        cout << ir[ir_to_name[right_register]].register_nonpointer_name.Type << endl;
    }
    else {
        string load_register;
        string icmp_register;

        /*===--- left ---===*/
        string left_nonpointer_type = ir[ir_to_name[left_register]].register_nonpointer_name.Type;
        string left_pointer_type = ir[ir_to_name[left_register]].register_pointer.Type;

        if (ir[ir_to_name[left_register]].Type == NOTPOINTER) {
            left_register = ir[ir_to_name[left_register]].register_nonpointer_name.Name;
            load_register = generate_regisuter_name();
            op_codes.push_back(IR_load(load_register, left_register, left_nonpointer_type, left_pointer_type, "8"));
            left_register = load_register;
        }
        /*===--- end ---===*/

        /*===--- right ---===*/
        string right_nonpointer_type = ir[ir_to_name[right_register]].register_nonpointer_name.Type;
        string right_pointer_type = ir[ir_to_name[right_register]].register_pointer.Type;

        if (left_nonpointer_type == "")
            left_nonpointer_type = right_nonpointer_type;

        if (ir[ir_to_name[right_register]].Type == NOTPOINTER) {
            right_register = ir[ir_to_name[right_register]].register_nonpointer_name.Name;
            load_register = generate_regisuter_name();
            op_codes.push_back(IR_load(load_register, right_register, right_nonpointer_type, right_pointer_type, "8"));
            right_register = load_register;
        }
        /*===--- end ---===*/

        map<const char, string> if_op = {{'<', "sgt"}, {'>', "slt"}};

        icmp_register = generate_regisuter_name();
        op_codes.push_back("\t" + icmp_register + " = icmp " + if_op[calc_op] +  " " + left_nonpointer_type + " " + left_register + ", " + right_register + "\n");

        result_register = icmp_register;
    }
}

void Generator::IR_calculation(const Node *node, vector<string> &op_codes, const char calc_op, string &result_register) {

    // ===--- common ---===
    string l_register = gen(node->left);
    string l_type     = node->left->type;

    string r_register = gen(node->right);
    string r_type     = node->right->type;

    // ===--- uncommon ---===
    string calc_register;
    string calc;

    if (l_register[0] != '%' && r_register[0] != '%') {
        int l_num = stoi(l_register);
        int r_num = stoi(r_register);

        switch (calc_op) {
            case '+':
                l_num += r_num;
                break;
            case '-':
                l_num -= r_num;
                break;
            case '*':
                l_num *= r_num;
                break;
            case '/':
                l_num /= r_num;
                break;
            default: {
                cout << "Token error : cant tokenize '" << calc_op << "'" << endl;
                exit(1);
                break;
            }
        }

        result_register = to_string(l_num);

        return;
    }

    switch (calc_op) {
        case '+':
            calc = "add nsw";
            break;
        case '-':
            calc = "sub nsw";
            break;
        case '*':
            calc = "mul nsw";
            break;
        case '/':
            calc = "sdiv";
            break;
        default: { cout << "Token error : cant tokenize '" << calc_op << "'" << endl;
            exit(1);
            break;
        }
    }

    op_codes.push_back("\n\t; " + calc + "\n");

    // if (l_register)
    // ===--- common ---===
    if (ir[ir_to_name[l_register]].Type == NOTPOINTER) {
        string type       = ir[ir_to_name[l_register]].register_pointer.Type;
        string load_register_1 = generate_regisuter_name();
        string load_register_2 = generate_regisuter_name();

        op_codes.push_back(IR_load(load_register_1, l_register, type, type + "*", "8"));

        type = ir[ir_to_name[l_register]].register_nonpointer_name.Type;

        op_codes.push_back(IR_load(load_register_2, load_register_1, type, type + "*", "4"));
        l_register = load_register_2;
    }

    // if (r_register)
    // ===--- common ---===
    if (ir[ir_to_name[r_register]].Type == NOTPOINTER) {
        string type       = ir[ir_to_name[r_register]].register_pointer.Type;
        string load_register_1 = generate_regisuter_name();
        string load_register_2 = generate_regisuter_name();

        op_codes.push_back(IR_load(load_register_1, r_register, type, type + "*", "8"));

        type = ir[ir_to_name[r_register]].register_nonpointer_name.Type;

        op_codes.push_back(IR_load(load_register_2, load_register_1, type, type + "*", "4"));
        r_register = load_register_2;
    }

    // ===--- common ---===
    calc_register = generate_regisuter_name();

    // ===--- uncommon ---===
    op_codes.push_back("\t" + calc_register + " = " + calc + " i32 " +
                       l_register + ", " + r_register + "\n\n");

    result_register                                      = calc_register;
    ir_to_name[result_register]                          = "calculation_tmp_register";
    ir["calculation_tmp_register"].register_pointer.Type = "i32";
    ir["calculation_tmp_register"].register_nonpointer_name.Type = "i32";
    ir["calculation_tmp_register"].register_pointer.Name = result_register;
    ir["calculation_tmp_register"].register_nonpointer_name.Name = result_register;
    ir["calculation_tmp_register"].Type = REALNUMBER;
}

void Generator::IR_mov(const Node *node, vector<string> &op_codes) {
    string alloca_p = generate_regisuter_name();
    string alloca_i = generate_regisuter_name();

    op_codes.push_back("\t; mov\n");

    // ===--- define ---===
    op_codes.push_back(
            "\t" + alloca_p + " = alloca i32*, align 8\n" + 
            "\t" + alloca_i + " = alloca i32, align 4\n" + 
            "\tstore i32* " + alloca_i + ", i32** " + alloca_p + ", align 8\n");

    // ===--- mov ---===
    string variable_name       = node->val;
    string store_register_name = gen(node->right);

    if ("i32" == trance_type(store_register_name))
        op_codes.push_back("\tstore i32 " + store_register_name + ", i32* " + alloca_i + ", align 4\n");

    else if (store_register_name[0] == '%') {
        //string load = generate_regisuter_name();
        op_codes.push_back(
        //        "\t" + load + " = load i32*, i32** " + alloca_p + ", align 8\n" + 
                "\tstore i32 " + store_register_name + ", i32* " + alloca_i + ", align 4\n");
    }

    op_codes.push_back("\n");

    // map ir -> ir_to_name
    ir[variable_name] = IR{Register{alloca_p, "i32*", "8"},
                           Register{alloca_i, "i32", "4"}, variable_name};

    ir_to_name.emplace(alloca_p, variable_name);
    ir_to_name.emplace(alloca_i, variable_name);
}

void Generator::IR_mov_reDefine(const Node *node) {
    // ===--- mov ---===
    string variable_name       = node->val;
    string alloca_p            = ir[variable_name].register_pointer.Name;
    string alloca_i            = ir[variable_name].register_nonpointer_name.Name;
    string store_register_name = gen(node->right);
    string load                = generate_regisuter_name();

    op_codes.push_back("\t" + load + " = load i32*, i32** " + alloca_p +
                       ", align 4\n" + "\tstore i32 " + store_register_name +
                       ", i32* " + load + ", align 4\n");

    op_codes.push_back("\n");
    return;
}

void Generator::IR_if(const Node *node, vector<string> &op_codes) {
    string icmp_register = gen(node->left);
    string if_reg, if_end_reg;
    string              end_register;

    vector<string> op_codes_tmp = op_codes;
    op_codes = {};

    if_reg = "if_" + to_string(if_frequency++);
    op_codes.push_back(if_reg + "_begin:\n"); 

    gen(node->right);

    end_register = if_reg + "_end";
    if (returned == false)
        op_codes.push_back("\tbr label %" + end_register + "\n\n");
    
    op_codes.push_back(end_register + ":\n"); 

    op_codes_tmp.push_back("\tbr i1 " + icmp_register +  ", label %" + if_reg + "_begin, label %" + end_register + "\n\n");
    op_codes.insert(op_codes.begin(), op_codes_tmp.begin(), op_codes_tmp.end());
    return;
}

void Generator::IR_while(const Node *node, vector<string> &op_codes) {
    vector<string> op_codes_tmp = op_codes;
    op_codes = {};

    /*
     * whileの中の条件を反転して最初で計算(ループの前にwhileするかどうかifする)
     * 分岐がfalseである(条件が成り立つ->反転->false) -> goto if_begin => goto while_end 
     * 分岐がtrueである(条件が成り立たない->反転->true)-> goto if_end
     *
     * while_begin: 
     *     (if bool)
     *
     * if_begin:
     *     goto while_end ;break
     *
     * if_end:
     *     (while data)
     *     goto while_begin
     *
     * while_end:
     *     (other)
     *
     */

    string while_register = "while_" + to_string(while_frequency++);
    op_codes.push_back(while_register + "_begin:\n"); 

    string icmp_register = gen(node->left);
    string if_register = "if_" + to_string(if_frequency) +"_begin";
    string end_register = "if_" + to_string(if_frequency++) +"_end";

    op_codes.push_back("\tbr i1 " + icmp_register +  ", label %" +  end_register + ", label %" + if_register + "\n\n");
    op_codes.push_back(if_register + ":\n");;
    gen(node->right);

    op_codes_tmp.push_back("\tbr label %" + while_register +  "_begin\n\n");
    op_codes.insert(op_codes.begin(), op_codes_tmp.begin(), op_codes_tmp.end());
    op_codes.push_back("\tbr label %" + while_register +  "_begin\n\n");
    op_codes.push_back(end_register + ":\n");;
    op_codes.push_back("\tbr label %while_" + to_string(while_frequency-1) + "_end\n\n");
    returned = true;

    op_codes.push_back(while_register + "_end:\n"); 

}

void Generator::IR___put__(const Node *node) {
    string put_register = gen(node->right);
    string load_register = generate_regisuter_name();
    string type = ir[ir_to_name[put_register]].register_nonpointer_name.Type;
    string string_declaration = "@.str." + to_string(string_declaration_number++);

    put_register = ir[ir_to_name[put_register]].register_nonpointer_name.Name;

    string string_declaration_sent = string_declaration +  " = private unnamed_addr constant [3 x i8] c\"%d\\00\", align 1\n";
    str_defines.push_back(string_declaration_sent);

    if (!defined_function["printf"]) {
        function_declaration.push_back("declare i32 @printf(i8* noundef, ...)\n");
        defined_function["printf"] = true;
    }

    string call_register = generate_regisuter_name();

    op_codes.push_back(
        "\t" + IR_load(load_register, put_register, type, type + "*", "4") + "\n"
        "\t" + call_register + " = call i32 (i8*, ...) @printf(" + 
            "i8* noundef getelementptr inbounds ([3 x i8], [3 x i8]* " + string_declaration + ", i64 0, i64 0), " +
            type +  " noundef " + load_register + 
        ")\n"
    );
    return;
}

string Generator::gen(const Node *node) {
    string result_register = "";

    if (node == NULL)
        return result_register;

    switch (node->kind) {
        case ND_NUM: {
            ir_to_name[node->val]  = "$_REAL_NUMBER";
            ir[ir_to_name[node->val]].Type = REALNUMBER;
            result_register = node->val;
            break;
        }

        case ND_VARIABLE: {
            env_register      = ir[node->val].register_pointer;
            env_register.Name = node->val;

            result_register   = ir[node->val].register_pointer.Name;
            ir[node->val].Type = NOTPOINTER;
            break;
        }

        case ND_LESS_THAN:
            IR_boolen(node, op_codes, '<', result_register);
            break;
        case ND_GREATER_THAN:
            IR_boolen(node, op_codes, '>', result_register);
            break;
        case ND_ADD:
            IR_calculation(node, op_codes, '+', result_register);
            break;
        case ND_SUB:
            IR_calculation(node, op_codes, '-', result_register);
            break;
        case ND_MUL:
            IR_calculation(node, op_codes, '*', result_register);
            break;
        case ND_DIV:
            IR_calculation(node, op_codes, '/', result_register);
            break;
        case ND_LET:
            IR_mov(node, op_codes);
            break;
        case ND_IF:
            IR_if(node, op_codes);
            break;
        case ND_WHILE:
            IR_while(node, op_codes);
            break;
        case ND___PUT__:
            IR___put__(node);
            break;
        case ND_MOV:
            IR_mov_reDefine(node);
            break;
        case ND_BREAK: {
            op_codes.push_back("\tbr label %while_" + to_string(while_frequency-1) + "_end\n\n");
            returned = true;
            gen(node->left);
            gen(node->right);
            break;
        }
        case ND_CALL_ARG: {
            string argment_register_name = gen(node->left);
            string type                  = ir[ir_to_name[argment_register_name]].register_nonpointer_name.Type;
            string argment_non_pointer   = ir[ir_to_name[argment_register_name]].register_nonpointer_name.Name;
            int    variable_type         = ir[ir_to_name[argment_register_name]].Type;
            string argment_register      = generate_regisuter_name();

        
            register_number--;
            if (variable_type == POINTER) {

                op_codes.push_back(IR_load(argment_register, argment_non_pointer, type, type+"*", "8"));

                argment_non_pointer = argment_register;
                argment_register    = generate_regisuter_name();
                register_number++;
            }

            if (variable_type != REALNUMBER)  {
                op_codes.push_back(IR_load(argment_register, argment_non_pointer, type, type+"*", "4"));
                register_number++;
            }

            else
                argment_register = argment_register_name;

            call_function_argments.push_back(argment_register);
            gen(node->right);
            break;
        }

        case ND_CALL_FUNCTION: {
            string argment_string;

            gen(node->right);

            result_register = generate_regisuter_name();

            for (auto tmp : call_function_argments) {
                argment_string += "i32 noundef " + tmp; 
                argment_string += ", ";
            }

            call_function_argments = {};
            argment_string.pop_back();
            argment_string.pop_back();

            op_codes.push_back(result_register + " = call i32 @" + node->val +
                               "(" + argment_string + ")\n");
            break;
        }
        case ND_RETURN: {
            returned = true;

            string return_value = gen(node->right);
            string type         = ir[ir_to_name[return_value]].register_pointer.Type;
            string load_register_pointer;
            string load_register_non_pointer = generate_regisuter_name();

            if (type == "i32*") {
                // 戻り値チェック!!!
                if (type != now_function_type + "*") {
                    cout << "Generator err: must to same return type as functions type. (-1)" << endl;
                    exit(1);
                }
                // if var
                load_register_pointer     = load_register_non_pointer;
                type         = ir[ir_to_name[return_value]].register_nonpointer_name.Type;
                op_codes.push_back(IR_load(load_register_pointer, ir[ir_to_name[return_value]].register_nonpointer_name.Name, type, type + "*", "4"));
                op_codes.push_back("\tret " + now_function_type + " " + load_register_pointer + "\n");
                break;
            }
            if (type == "i32**") {
                // if var
                type         = type.substr(0, 3);
                return_value = ir[ir_to_name[return_value]].register_nonpointer_name.Name;
                op_codes.push_back(IR_load(load_register_non_pointer, return_value, type,
                                           type + "*", "8"));
                return_value = load_register_non_pointer;
                op_codes.push_back("\tret " + now_function_type + " " +
                                   load_register_non_pointer + "\n");
                break;
            }
            op_codes.push_back("\tret " + now_function_type + " " + return_value + "\n");
            break;
        }
        case ND_BLOCK: {
            gen(node->left);
            gen(node->right);
            break;
        }
        case ND_FN: {
            string function_name = node->val;
            string argment_string;
            string ir_argment_string;
            now_function_type = type_to_i_type[node->type];

            if (function_name == "main" && now_function_type != "i32") {
                cout << "Generator err: must to be return int by main. (-1)" << endl;
                exit(1);
            }

            typedef struct  {
                string Name;
                string Type;
                string reg;
            } Argment_Name_Type_Register;

            vector<Argment_Name_Type_Register> reg_argment_vec;

            for (auto tmp : node->argments) {
                string type = trance_type(tmp.Type);// + "*";
                string reg_argment      = generate_regisuter_name();
                // argment string
                argment_string += type + " noundef " + reg_argment;
                argment_string += ", ";

                reg_argment_vec.push_back({tmp.Name, tmp.Type, reg_argment});
            }


            for (auto tmp : reg_argment_vec) {
                string type = trance_type(tmp.Type);// + "*";
                string alloca_i      = generate_regisuter_name();
                string alloca_p      = generate_regisuter_name();
                string variable_name = tmp.Name;

                /* ir_argment_string settings */
                // map ir -> ir_to_name
                ir_to_name.emplace(alloca_i, variable_name);
                ir_to_name.emplace(alloca_p, variable_name);

                ir[ir_to_name[alloca_i]].register_nonpointer_name = {alloca_i, type};
                ir[ir_to_name[alloca_p]].register_pointer = {alloca_p, type + "*"};

                ir_argment_string.append(
                        // 引数を関数内の変数に代入
                        "\t" + alloca_i + " = alloca " + type + ", align 4\n" + 
                        "\tstore " + type + " " + tmp.reg + ", " + type + "* " + alloca_i  + ", align 4\n"

                        // 上で作った変数のポインタ変数を作成
                        "\t" + alloca_p + " = alloca " + type + "*, align 8\n" + 
                        "\tstore " + type + "* " + alloca_i + ", " + type + "** " + alloca_p + ", align 8\n"
                );

                /* ===--- end ---=== */

            }

            argment_string.pop_back();
            argment_string.pop_back();

            op_codes.push_back("define " + now_function_type + " @" + function_name + "(" + argment_string + ") #0 {\nentry:\n");
            op_codes.push_back("\t; argments\n");
            op_codes.push_back(ir_argment_string + "\n\n");
            gen(node->right);
            op_codes.push_back("}\n\n");
            register_number = 0;
            ir_to_name      = {};
            returned = false;
            break;
        }
        default: {
            break;
        }
    }
    return result_register;
}

vector<string> Generator::codegen(vector<Node *> &nodes) {
    register_number = 0;
    string_declaration_number = 0;
    if_frequency = 0;
    while_frequency = 0;
    returned = false;
    type_to_i_type  = {
         {"int", "i32"},
         {"long", "i64"},
         {"void", "void"},
         {"str", "i8"},
    };

    for (Node *node : nodes) {
        gen(node);
    }

    op_codes.insert(op_codes.begin(), str_defines.begin(), str_defines.end());
    op_codes.insert(op_codes.end(), function_declaration.begin(), function_declaration.end());
    return op_codes;
}
