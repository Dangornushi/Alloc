#include "Alloc.h"

string Generator::generate_regisuter_name(void) {
    return "%" + to_string(register_number++);
}

string Generator::trance_type(string reg) {
    string ret;

    (reg.at(0) == '%') ? ret = ir[ir_to_name[reg]].register_pointer.Type : ret = "i32";

    return ret;
}

string Generator::IR_load(string load_register, string loaded_register, string type_1, string type_2, string size) {
    return "\t" + load_register + " = load " + type_1 + ", " + type_2 + " " + loaded_register + ", align " + size + "\n";
}

void Generator::IR_calculation(const Node *node, vector<string> &op_codes, const char calc_op, string &result_register) {
    // ===--- common ---===
    string l_register = gen(node->left);
    string r_register = gen(node->right);
    string type       = ir[ir_to_name[l_register]].register_pointer.Type;

    // ===--- uncommon ---===
    string calc_register;
    string calc;

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
        default:
            break;
    }

    op_codes.push_back("\n\t; " + calc + "\n");

    // ===--- common ---===
    if (ir[ir_to_name[l_register]].register_pointer.Type == "i32*") {
        string load_register_1 = generate_regisuter_name();
        string load_register_2 = generate_regisuter_name();

        op_codes.push_back(IR_load(load_register_1, l_register, type, type + "*", "8"));
        type = ir[ir_to_name[l_register]].register_nonpointer_name.Type;
        op_codes.push_back(IR_load(load_register_2, load_register_1, type, type + "*", "4"));
        l_register = load_register_2;
    }

    // ===--- common ---===
    calc_register = generate_regisuter_name();

    // ===--- uncommon ---===
    op_codes.push_back("\t" + calc_register + " = " + calc + " i32 " + l_register + ", " + r_register + "\n\n");
    result_register = calc_register;
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
    string variable_name       = node->left->val;
    string store_register_name = gen(node->right);

    if ("i32" == trance_type(store_register_name)) {
        op_codes.push_back("\tstore i32 " + store_register_name + ", i32* " + alloca_i + ", align 4\n");

    }

    else if (store_register_name[0] == '%') {
        string load = generate_regisuter_name();
        op_codes.push_back(
            "\t" + load + " = load i32*, i32** " + alloca_p + ", align 8\n" +
            "\tstore i32 " + store_register_name + ", i32* " + load + ", align 4\n");
    }

    op_codes.push_back("\n");

    // map ir -> ir_to_name
    ir.emplace(variable_name, IR{Register{alloca_p, "i32*", "8"}, Register{alloca_i, "i32", "4"}, ""});
    ir_to_name.emplace(alloca_i, variable_name);
    ir_to_name.emplace(alloca_p, variable_name);
}

string Generator::gen(const Node *node) {
    string result_register = "";

    if (node == NULL)
        return result_register;

    switch (node->kind) {
        case ND_NUM: {
            result_register = node->val;
            break;
        }
        case ND_VARIABLE: {
            env_register      = ir[node->val].register_pointer;
            env_register.Name = node->val;

            result_register   = ir[node->val].register_pointer.Name;
            break;
        }
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
        case ND_RETURN: {
            string return_value = gen(node->right);
            string type         = ir[ir_to_name[return_value]].register_pointer.Type;
            string load_register_pointer;
            string load_register_non_pointer = generate_regisuter_name();

            op_codes.push_back("\t; return\n");
            if (type == "i32*") {
                // if var
                load_register_pointer     = load_register_non_pointer;
                load_register_non_pointer = generate_regisuter_name();
                op_codes.push_back(IR_load(load_register_pointer, return_value, type, type + "*", "8"));
                type         = ir[ir_to_name[return_value]].register_nonpointer_name.Type;
                return_value = load_register_pointer;
                op_codes.push_back(IR_load(load_register_non_pointer, return_value, type, type + "*", "4"));
                op_codes.push_back("\tret i32 " + load_register_non_pointer + "\n");
                break;
            }
            // if const Num
            op_codes.push_back("\tret i32 " + return_value + "\n");
            break;
        }
        case ND_BLOCK: {
            gen(node->left);
            gen(node->right);
            break;
        }
        case ND_FN: {
            string function_name = node->left->val;
            op_codes.push_back("define i32 @" + function_name + "() #0 {\nentry:\n");
            gen(node->right);
            op_codes.push_back("}");
            break;
        }
        default:
            break;
    }
    return result_register;
}

vector<string> Generator::codegen(vector<Node *> &nodes) {
    register_number = 0;

    for (Node *node : nodes) {
        gen(node);
    }

    return op_codes;
}
