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

    struct List_cell {
        json info;
        List_cell* next;
    };
    typedef List_cell* List;

    struct Dictionary_cell {
        std::pair<std::string, json> info;
        Dictionary_cell* next;
    };
    typedef Dictionary_cell* Dict;

    char type;
    double num;
    bool boolean;
    std::string string;
    List l_front, l_back;
    Dict d_front, d_back;
};

// -------------------- ITERATORS --------------------
struct json::list_iterator {
public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = json;
    using pointer = json*;
    using reference = json&;
    list_iterator(impl::List);

    reference operator*() const {return ptr->info;}
    pointer operator->() const {return &(ptr->info);}
    list_iterator& operator++() {ptr = ptr->next; return *this;}
    list_iterator operator++(int) {list_iterator it = {ptr}; ++(*this); return it;}
    bool operator==(list_iterator const& rhs) const {return ptr == rhs.ptr;}
    bool operator!=(list_iterator const& rhs) const {return ptr != rhs.ptr;}
    list_iterator begin_list() {return list_iterator{(*this)->pimpl->l_front};}
    list_iterator end_list() {return list_iterator{nullptr};}

private:
    impl::List ptr;
};

// -------------------- JSON PUBLIC METHODS --------------------
json::json() {
    std::cout << "costruttore" << std::endl;
    pimpl = new impl;
}
json::json(json const& rhs) : json() {
    std::cout << "copy constructor" << std::endl;
    if (rhs.is_list()) { // TODO: implementare con iteratori
        auto ptr = rhs.pimpl->l_front;
        while (ptr) {
            push_back(ptr->info);
            ptr = ptr->next;
        }
    } else if (rhs.is_string()) {
        set_string(rhs.get_string());
    }
}

json::json(json&&) {std::cout << "move constructor" << std::endl;}
json::~json() {std::cout << "distruttore" << std::endl;}
json& json::operator=(json const& rhs) { // TODO
    std::cout << "ASSIGN" << std::endl;
    if (this != &rhs) {
        // TODO fare il set_null() per svuotare qualsiasi contenuto
        auto ptr = rhs.pimpl->l_front;
        while (ptr) {
            push_back(ptr->info);
            ptr = ptr->next;
        }
    }
    return *this;
}
json& json::operator=(json&& rhs) {std::cout << "aaaaaa" << std::endl; return rhs;} // TODO

bool json::is_list() const {return pimpl->type == '[';}
bool json::is_dictionary() const {return pimpl->type == '{';}
bool json::is_string() const {return pimpl->string != "\0";}
bool json::is_number() const {return pimpl->num != pimpl->inf;}
bool json::is_bool() const {return pimpl->type == 'b';}
bool json::is_null() const {return pimpl->type == '\0' and  pimpl->num == pimpl->inf and pimpl->string == "\0" and pimpl->l_front== nullptr and pimpl->d_front == nullptr;}
double& json::get_number() {return pimpl->num;}
double const& json::get_number() const {return pimpl->num;}
bool& json::get_bool() {return pimpl->boolean;}
bool const& json::get_bool() const {return pimpl->boolean;}
std::string& json::get_string() {return pimpl->string;}
std::string const& json::get_string() const {return pimpl->string;}
void json::set_string(std::string const& s) {pimpl->string = s;}
void json::set_bool(bool b) {pimpl->boolean = b; pimpl->type = 'b';}
void json::set_number(double n) {pimpl->num = n;}
void json::set_null() {} // TODO: chiamare prima di ogni set
void json::set_list() {pimpl->type = '[';} // soluzione temporanea che usa type per discriminare se è una lista
void json::set_dictionary() {pimpl->type = '{';} // soluzione temporanea che usa type per discriminare se è un dizionario

void json::push_front(json const& j) {
    if (is_list()) {
        if (pimpl->l_front == nullptr) {
            pimpl->l_front = pimpl->l_back = new impl::List_cell{j, nullptr};
        } else {
            pimpl->l_front = new impl::List_cell{j, pimpl->l_front};
        }
    } else {
        std::cout << "Push front fallito: non una lista" << std::endl; // TODO: sostituire con eccezione
    }
}
void json::push_back(json const& j) {
    if (is_list()) {
        if (pimpl->l_front == nullptr) {
            push_front(j);
        } else {
            pimpl->l_back->next = new impl::List_cell {j, nullptr};
            pimpl->l_back = pimpl->l_back->next;
        }
    } else {
        std::cout << "Push back fallito: non una lista" << std::endl; // TODO: sostituire con eccezione
    }
}
void json::insert(std::pair<std::string,json> const&) {}









// -------------------- TEST --------------------
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
    j.set_string("ciao");

    check_type(j);
    return 0;
}
