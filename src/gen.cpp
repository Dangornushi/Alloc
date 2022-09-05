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
        case ND_ADD: {
            string l_register = gen(node->left);
            string r_register = gen(node->right);
            op_codes.push_back("\tadd " + l_register + ", " + r_register + "\n");
            result_register = l_register;
            break;
        }
        case ND_LET: {
            string alloca_p            = generate_regisuter_name();
            string alloca_i            = generate_regisuter_name();

            string variable_name       = node->left->val;
            string store_register_name = gen(node->right);

            // ===--- define ---===
            op_codes.push_back(
                "\t" + alloca_p + " = alloca i32*, align 8\n" +
                "\t" + alloca_i + " = alloca i32, align 4\n" +
                "\tstore i32* " + alloca_i + ", i32** " + alloca_p + ", align 8\n");

            // ===--- mov ---===

            if ("i32" == trance_type(store_register_name)) {
                op_codes.push_back("\tstore i32 " + store_register_name + ", i32* " + alloca_i + ", align 4\n");

            }

            else if ("i32*" == trance_type(store_register_name)) {
                string load = generate_regisuter_name();
                op_codes.push_back(
                    "\t" + load + " = load i32*, i32** " + store_register_name + ", align 8\n" +
                    "\tstore i32* " + load + ", i32** " + alloca_p + ", align 4\n");
            }

            op_codes.push_back("\n");

            // map ir -> ir_to_name
            ir.emplace(variable_name, IR{Register{alloca_p, "i32*", "8"}, Register{alloca_i, "i32", "4"}, ""});
            ir_to_name.emplace(alloca_i, variable_name);
            ir_to_name.emplace(alloca_p, variable_name);

            break;
        }
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
        default:
            break;
    }
    return result_register;
}

vector<string> Generator::codegen(vector<Node *> &nodes) {
    register_number = 0;
    op_codes.push_back("define i32 @main() #0 {\nentry:\n");

    for (Node *node : nodes) {
        gen(node);
    }

    op_codes.push_back("}");
    return op_codes;
}
