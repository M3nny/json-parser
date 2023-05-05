#include "json.hpp"

std::istream& operator>>(std::istream& lhs, json& rhs) { // TODO
    char c;
    lhs >> c; // salto gli spazi e leggo
    if (c >= '0' and c <= '9') {
        lhs.putback(c);
        lhs >> rhs.get_number();
    } else {
        // lhs >> rhs.pimpl->type;
    }
    return lhs;
}

// -------------------- IMPLEMENTATION --------------------
struct json::impl {
public:
    static constexpr double inf = std::numeric_limits<double>::max();
    impl() : type('\0'), num(inf), boolean(false), string("\0"), l_front(nullptr), l_back(nullptr), d_front(nullptr), d_back(nullptr) {}

    template<typename T>
    struct Cell {
        T info;
        Cell* next;
    };
    typedef Cell<json>* List;
    typedef Cell<std::pair<std::string, json>>* Dict;

    bool empty() const {
        if (is_list()) {
            return l_front == nullptr;
        } else if (is_dictionary()) {
            return d_front == nullptr;
        } else {
            std::cout << "Funzione empty usata su un json che non è una lista o un dizionario, terminazione programma" << std::endl; // TODO: sostituire con eccezione
            exit(0);
        }
    }

	bool is_list() const {return type == '[';}
	bool is_dictionary() const {return type == '{';}
	bool is_string() const {return string != "\0";}
	bool is_number() const {return num != inf;}
	bool is_bool() const {return type == 'b';}
	bool is_null() const {return type == '\0' and  num == inf and string == "\0" and l_front== nullptr and d_front == nullptr;}
    double& get_number() {return num;}
	double const& get_number() const {return num;}
	bool& get_bool() {return boolean;}
	bool const& get_bool() const {return boolean;}
	std::string& get_string() {return string;}
	std::string const& get_string() const {return string;}
	void set_string(std::string const& s) {string = s;}
	void set_bool(bool b) {boolean = b; type = 'b';}
	void set_number(double n) {num = n;}
	void set_null(); // TODO: chiamare prima di ogni set
	void set_list() {type = '[';} // soluzione temporanea che usa type per discriminare se è una lista
	void set_dictionary() {type = '{';} // soluzione temporanea che usa type per discriminare se è un dizionario

	void push_front(json const& j) {
        if (is_list()) {
            if (empty()) {
                l_front = l_back = new Cell<json> {j, nullptr}; // TODO: fare copy constructor
                return;
            }
            std::cout << "inserisco con lista non vuota" << std::endl;
            l_front = new Cell<json> {j, l_front};
        } else {
            std::cout << "Push front fallito: non una lista" << std::endl; // TODO: sostituire con eccezione
        }
    }
	void push_back(json const& j) {
        if (is_list()) {
            if (empty()) {
                push_front(j);
                return;
            }
            l_back->next = new Cell<json> {j, nullptr};
            l_back = l_back->next;
        } else {
            std::cout << "Push back fallito: non una lista" << std::endl; // TODO: sostituire con eccezione
        }
    }
	void insert(std::pair<std::string,json> const&); // TODO


private:
    char type;

    double num;
    bool boolean;
    std::string string;
    List l_front, l_back;
    Dict d_front, d_back;
};

// -------------------- JSON PUBLIC METHODS --------------------
json::json() {
    std::cout << "costruttore" << std::endl;
    pimpl = new impl;
}
json::json(json const&) {std::cout << "copy constructor" << std::endl;}
json::json(json&&) {std::cout << "move constructor" << std::endl;}
json::~json() {std::cout << "distruttore" << std::endl;}

bool json::is_list() const {return pimpl->is_list();}
bool json::is_dictionary() const {return pimpl->is_dictionary();}
bool json::is_string() const {return pimpl->is_string();}
bool json::is_number() const {return pimpl->is_number();}
bool json::is_bool() const {return pimpl->is_bool();}
bool json::is_null() const {return pimpl->is_null();}
double& json::get_number() {return pimpl->get_number();}
double const& json::get_number() const {return pimpl->get_number();}
bool& json::get_bool() {return pimpl->get_bool();}
bool const& json::get_bool() const {return pimpl->get_bool();}
std::string& json::get_string() {return pimpl->get_string();}
std::string const& json::get_string() const {return pimpl->get_string();}
void json::set_string(std::string const& s) {pimpl->set_string(s);}
void json::set_bool(bool b) {pimpl->set_bool(b);}
void json::set_number(double n) {pimpl->set_number(n);}
void json::set_null() {} // TODO
void json::set_list() {pimpl->set_list();}
void json::set_dictionary() {} // TODO
void json::push_front(json const& j) {pimpl->push_front(j);}
void json::push_back(json const& j) {pimpl->push_back(j);}





// -------------------- TEST --------------------
void check_type(json& j) { // copy constructor non ancora definito
    std::cout << "number? " << j.is_number() << std::endl;
    std::cout << "boolean? " << j.is_bool() << std::endl;
    std::cout << "string? " << j.is_string() << std::endl;
    std::cout << "list? " << j.is_list() << std::endl;
    std::cout << "dictionary? " << j.is_dictionary() << std::endl;
    std::cout << "null? " << j.is_null() << std::endl;
}
int main () {
    json j;
    j.set_string("ciao");

    check_type(j);
    std::cout << "Actual value: " << j.get_string() << std::endl;
    return 0;
}
