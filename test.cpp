#include "json.cpp"

void check_type(json& j) {
    std::cout << "number? " << j.is_number() << std::endl;
    std::cout << "boolean? " << j.is_bool() << std::endl;
    std::cout << "string? " << j.is_string() << std::endl;
    std::cout << "list? " << j.is_list() << std::endl;
    std::cout << "dictionary? " << j.is_dictionary() << std::endl;
    std::cout << "null? " << j.is_null() << std::endl;
}

int main () {
    json j;
    std::ifstream is("input.json");

    try {
        is >> j;
    } catch (json_exception& e) {
        std::cout << e.msg << std::endl;
        exit(0);
    }

    try {
        std::cout << j;
    } catch (json_exception& e) {
        std::cout << e.msg << std::endl;
        exit(0);
    }
    return 0;
}
