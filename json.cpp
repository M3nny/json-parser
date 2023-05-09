#include "json.hpp"

void J(std::istream&, json&);

void L(std::istream& is, json& j) {
    char c;
    do {
        J(is, j);
        is >> c;
    } while (c == ',');
    is.putback(c);
}

void J(std::istream& is, json& j) {
    char c;
    is >> c; // salto gli spazi e leggo
    if (c >= '0' and c <= '9') { // num
        std::string s_num;
        s_num.push_back(c);
        while ((is >> c) and (c >= '0' and c <= '9') or c == '.') {
            s_num.push_back(c);
        }
        is.putback(c);
        double num = stod(s_num);
        // debug: std::cout << num << std::endl;
        // creo json: num
    } else if (c == 't'){ // bool->true
        std::string target = "true";
        for (int i = 1; i < target.length(); i++) {
            is >> c;
            if (c != target.at(i)) {
                std::cout << target.at(i) << " expected, found: " << c << std::endl;
                exit(0);
            }
        }
        // creo json: bool->true
    } else if (c == 'f') { // bool->false
        std::string target = "false";
        for (int i = 1; i < target.length(); i++) {
            is >> c;
            if (c != target.at(i)) {
                std::cout << target.at(i) << " expected, found: " << c << std::endl;
                exit(0);
            }
        }
        // creo json: bool->false
    } else if (c == 'n') { // null
        std::string target = "null";
        for (int i = 1; i < target.length(); i++) {
            is >> c;
            if (c != target.at(i)) {
                std::cout << target.at(i) << " expected, found: " << c << std::endl;
                exit(0);
            }
        }
        // creo json: null
    } else if (c == '\0') {
        std::cout << "[" << char(is.peek()) << "]" << std::endl;
        // creo json: null
    } else if (c == '"') { // string
        std::string s;
        is.get(c);
        while (c != '"') {
            if (is.eof()) {
                std::cout << "reached eof, without finding closing \"" << std::endl;
                exit(0);
            }
            if (c == '\\') {
                s.push_back(c);
                is.get(c);
                if (c == '"') {
                    s.push_back(c);
                    is.get(c);
                }
            } else {
                s.push_back(c);
                is.get(c);
            }
        }
        // debug: std::cout << s << std::endl;

    } else if (c == '[') {
        // TODO: gestire lista [ ]
        if (is.peek() == ']') { // lista vuota
            is >> c;
        } else {
            L(is, j);
            is >> c;
            if (c != ']') {
                std::cout << "closing ] expected, found: " << c << std::endl;
                exit(0);
            }
        }
    } else {
        is.putback(c);
        std::cout << "unknown symbol: " << c << std::endl;
        exit(0);
    }
}

std::istream& operator>>(std::istream& lhs, json& rhs) {
    char c;
    J(lhs, rhs);
    lhs >> c; // mangio l'ultimo carattere newline
    if (lhs.eof()) return lhs;
    else {
        std::cout << "istream not empty after parsing" << std::endl;
        exit(0);
    }
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

// -------------------- ITERATORS -------------------- lanciare eccezione se chiamati su json che non sono liste o dizionari
struct json::list_iterator {
public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = json;
    using pointer = json*;
    using reference = json&;

    list_iterator(impl::List_cell* l) : ptr(l) {}
    reference operator*() const {return ptr->info;}
    pointer operator->() const {return &(ptr->info);}
    list_iterator& operator++() {ptr = ptr->next; return *this;}
    list_iterator operator++(int) {list_iterator it = {ptr}; ++(*this); return it;}
    bool operator==(list_iterator const& rhs) const {return ptr == rhs.ptr;}
    bool operator!=(list_iterator const& rhs) const {return ptr != rhs.ptr;}
    list_iterator begin_list() {return list_iterator{(*this)->pimpl->l_front};}
    list_iterator end_list() {return list_iterator{(*this)->pimpl->l_back};}

private:
    impl::List_cell* ptr;
};

struct json::dictionary_iterator {
    using iterator_category = std::forward_iterator_tag;
    using value_type = std::pair<std::string, json>;
    using pointer = std::pair<std::string, json>*;
    using reference = std::pair<std::string, json>&;

    dictionary_iterator(impl::Dictionary_cell* d) : ptr(d) {}
    reference operator*() const {return ptr->info;}
    pointer operator->() const {return &(ptr->info);}
    dictionary_iterator& operator++() {ptr = ptr->next; return *this;}
    dictionary_iterator operator++(int) {dictionary_iterator it = {ptr}; ++(*this); return it;}
    bool operator==(dictionary_iterator const& rhs) const {return ptr == rhs.ptr;}
    bool operator!=(dictionary_iterator const& rhs) const {return ptr != rhs.ptr;}
    dictionary_iterator begin_dictionary() {return dictionary_iterator{(*this)->second.pimpl->d_front};} // !!
    dictionary_iterator end_dictionary() {return dictionary_iterator{(*this)->second.pimpl->d_back};} // !!
private:
    impl::Dictionary_cell* ptr;
};

struct json::const_list_iterator {
    using iterator_category = std::forward_iterator_tag;
    using value_type = const json;
    using pointer = const json*;
    using reference = const json&;

    const_list_iterator(impl::List_cell* l) : ptr(l) {}
    reference operator*() const {return ptr->info;}
    pointer operator->() const {return &(ptr->info);}
    const_list_iterator& operator++() {ptr = ptr->next; return *this;}
    const_list_iterator operator++(int) {const_list_iterator it = {ptr}; ++(*this); return it;}
    bool operator==(const_list_iterator const& rhs) const {return ptr == rhs.ptr;}
    bool operator!=(const_list_iterator const& rhs) const {return ptr != rhs.ptr;}
    const_list_iterator begin_list() const {return const_list_iterator{(*this)->pimpl->l_front};}
    const_list_iterator end_list() const {return const_list_iterator{(*this)->pimpl->l_back};}

private:
    impl::List_cell* ptr;
};

struct json::const_dictionary_iterator {
    using iterator_category = std::forward_iterator_tag;
    using value_type = const std::pair<std::string, json>;
    using pointer = const std::pair<std::string, json>*;
    using reference = const std::pair<std::string, json>&;

    const_dictionary_iterator(impl::Dictionary_cell* d) : ptr(d) {}
    reference operator*() const {return ptr->info;}
    pointer operator->() const {return &(ptr->info);}
    const_dictionary_iterator& operator++() {ptr = ptr->next; return *this;}
    const_dictionary_iterator operator++(int) {const_dictionary_iterator it = {ptr}; ++(*this); return it;}
    bool operator==(const_dictionary_iterator const& rhs) const {return ptr == rhs.ptr;}
    bool operator!=(const_dictionary_iterator const& rhs) const {return ptr != rhs.ptr;}
    const_dictionary_iterator begin_dictionary() const {return const_dictionary_iterator{(*this)->second.pimpl->d_front};} // !!
    const_dictionary_iterator end_dictionary() const {return const_dictionary_iterator{(*this)->second.pimpl->d_back};} // !!
private:
    impl::Dictionary_cell* ptr;
};


// -------------------- JSON PUBLIC METHODS --------------------
json::json() {
    std::cout << "CONSTRUCTOR" << std::endl;
    pimpl = new impl;
}
json::json(json const& rhs) : json() {
    std::cout << "COPY CONSTRUCTOR" << std::endl;
    if (rhs.is_number()) {
        set_number(rhs.get_number());
    } else if (rhs.is_bool()) {
        set_bool(rhs.is_bool());
    } else if (rhs.is_string()) {
        set_string(rhs.get_string());
    } else if (rhs.is_list()) {
        auto ptr = rhs.pimpl->l_front;
        while (ptr) {
            push_back(ptr->info);
            ptr = ptr->next;
        }
    } else if (rhs.is_dictionary()) {
        auto ptr = rhs.pimpl->d_front;
        while (ptr) {
            insert(ptr->info);
            ptr = ptr->next;
        }
    }
}
json::json(json&& rhs) {
    std::cout << "MOVE CONSTRUCTOR" << std::endl;
    *this = std::move(rhs);
}
json::~json() {
    std::cout << "DESTRUCTOR" << std::endl;
    set_null();
    delete pimpl;
}
json& json::operator=(json const& rhs) {
    std::cout << "COPY ASSIGNMENT" << std::endl;
    if (this != &rhs) {
        if (rhs.is_number()) {
            set_number(rhs.get_number());
        } else if (rhs.is_bool()) {
            set_bool(rhs.is_bool());
        } else if (rhs.is_string()) {
            set_string(rhs.get_string());
        } else if (rhs.is_list()) { // TODO: implementare con iteratori
            auto ptr = rhs.pimpl->l_front;
            while (ptr) {
                push_back(ptr->info);
                ptr = ptr->next;
            }
        } else if (rhs.is_dictionary()) {
            auto ptr = rhs.pimpl->d_front;
            while (ptr) {
                insert(ptr->info);
                ptr = ptr->next;
            }
        }
    }
    return *this;
}
json& json::operator=(json&& rhs) {
    std::cout << "MOVE ASSIGNMENT" << std::endl;
    if (this != &rhs) {
        set_null();
        pimpl->l_front = rhs.pimpl->l_front;
        pimpl->l_back = rhs.pimpl->l_back;
        pimpl->d_front = rhs.pimpl->d_front;
        pimpl->d_back = rhs.pimpl->d_back;
        rhs.pimpl->l_front = rhs.pimpl->l_back = nullptr;
        rhs.pimpl->d_front = rhs.pimpl->d_back = nullptr;
    }
    return *this;
}

json const& json::operator[](std::string const& s) const{
    if (is_dictionary()) {
        auto ptr = pimpl->d_front;
        while (ptr) {
            if (ptr->info.first == s) {
                return ptr->info.second;
            }
            ptr = ptr->next;
        }
        std::cout << "Key non trovata usando operator[] impossibile inserire" << std::endl;
        exit(0);
    }
    std::cout << "Operator[] chiamato su un json che non era un dizionario" << std::endl;
    exit(0);
}
json& json::operator[](std::string const& s) {
    json* j = new json;
    if (is_dictionary()) {
        auto ptr = pimpl->d_front;
        while (ptr) {
            if (ptr->info.first == s) {
                return ptr->info.second;
            }
            ptr = ptr->next;
        }
        std::pair<std::string, json> pair{"key", *j};
        insert(pair);
        return *j;
    }
    std::cout << "Operatore [] chiamato su un json che non era un dizionario" << std::endl;
    exit(0);
}

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

void json::set_string(std::string const& s) {set_null(); pimpl->string = s;}
void json::set_bool(bool b) {set_null(); pimpl->boolean = b; pimpl->type = 'b';}
void json::set_number(double n) {set_null(); pimpl->num = n;}
void json::set_list() {set_null(); pimpl->type = '[';} // soluzione temporanea che usa type per discriminare se è una lista
void json::set_dictionary() {set_null(); pimpl->type = '{';} // soluzione temporanea che usa type per discriminare se è un dizionario
void json::set_null() {
    if (is_list()) {
        auto ptr = pimpl->l_front;
        while (ptr) {
            ptr->info.set_null();
            auto next = ptr->next;
            delete ptr;
            ptr = next;
        }
        pimpl->type = '\0';
        pimpl->l_front = nullptr;
        pimpl->l_back = nullptr;
    } else if (is_dictionary()) {
        auto ptr = pimpl->d_front;
        while (ptr) {
            ptr->info.second.set_null();
            auto next = ptr->next;
            delete ptr;
            ptr = next;
        }
        pimpl->type = '\0';
        pimpl->d_front = nullptr;
        pimpl->d_back = nullptr;
    } else {
        pimpl->num = pimpl->inf;
        pimpl->boolean = false;
        pimpl->type = '\0';
        pimpl->string = "\0";
    }
}

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
            pimpl->l_back->next = new impl::List_cell{j, nullptr};
            pimpl->l_back = pimpl->l_back->next;
        }
    } else {
        std::cout << "Push back fallito: non una lista" << std::endl; // TODO: sostituire con eccezione
    }
}
void json::insert(std::pair<std::string,json> const& j) {
    if (is_dictionary()) {
        if (pimpl->d_front == nullptr) {
            pimpl->d_front = pimpl->d_back = new impl::Dictionary_cell{j, nullptr};
        } else {
            pimpl->d_back->next = new impl::Dictionary_cell{j, nullptr};
            pimpl->d_back = pimpl->d_back->next;
        }
    } else {
        std::cout << "Insert fallito: non un dizionario" << std::endl; // TODO: sostituire con eccezione
    }
}
