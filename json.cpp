#include "json.hpp"

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

// -------------------- CONSTRUCTORS - DESTRUCTOR --------------------
json::json() { // CONSTRUCTOR
    pimpl = new impl;
}
json::json(json const& rhs) : json() { // COPY CONSTRUCTOR
    *this = rhs;
}
json::json(json&& rhs) : json() { // MOVE CONSTRUCTOR
    *this = std::move(rhs);
}
json::~json() { // DESTRUCTOR
    set_null();
    delete pimpl;
}

// -------------------- ITERATORS --------------------
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
private:
    impl::List_cell* ptr;
};
json::list_iterator json::begin_list() {
    if (is_list()) {
        return list_iterator{pimpl->l_front};
    } else {
        throw json_exception{"begin_list() called on a non list json object"};
    }
}
json::list_iterator json::end_list() {
    if (is_list()) {
        return list_iterator{nullptr};
    } else {
        throw json_exception{"end_list() called on a non list json object"};
    }
}

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
private:
    impl::Dictionary_cell* ptr;
};
json::dictionary_iterator json::begin_dictionary() {
    if (is_dictionary()) {
        return dictionary_iterator{pimpl->d_front};
    } else {
        throw json_exception{"begin_dictionary() called on a non dictionary json object"};
    }
}
json::dictionary_iterator json::end_dictionary() {
    if (is_dictionary()) {
        return dictionary_iterator{nullptr};
    } else {
        throw json_exception{"end_dictionary() called on a non dictionary json object"};
    }
}

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
private:
    impl::List_cell* ptr;
};
json::const_list_iterator json::begin_list() const {
    if (is_list()) {
        return const_list_iterator{pimpl->l_front};
    } else {
        throw json_exception{"begin_list() const called on a non list json object"};
    }
}
json::const_list_iterator json::end_list() const {
    if (is_list()) {
        return const_list_iterator{nullptr};
    } else {
        throw json_exception{"end_list() const called on a non list json object"};
    }
}

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
private:
    impl::Dictionary_cell* ptr;
};
json::const_dictionary_iterator json::begin_dictionary() const {
    if (is_dictionary()) {
        return const_dictionary_iterator{pimpl->d_front};
    } else {
        throw json_exception{"begin_dictionary() const called on a non dictionary json object"};
    }
}
json::const_dictionary_iterator json::end_dictionary() const {
    if (is_dictionary()) {
        return const_dictionary_iterator{nullptr};
    } else {
        throw json_exception{"end_dictionary() const called on a non dictionary json object"};
    }
}

// -------------------- BOOL METHODS --------------------
bool json::is_list() const {return pimpl->type == '[';}
bool json::is_dictionary() const {return pimpl->type == '{';}
bool json::is_string() const {return pimpl->string != "\0";}
bool json::is_number() const {return pimpl->num != pimpl->inf;}
bool json::is_bool() const {return pimpl->type == 'b';}
bool json::is_null() const {return pimpl->type == '\0' and  pimpl->num == pimpl->inf and pimpl->string == "\0" and pimpl->l_front== nullptr and pimpl->d_front == nullptr;}

// -------------------- JSON METHODS --------------------
double& json::get_number() {
    if (is_number()) return pimpl->num;
    else throw json_exception{"get_number() called on a non number json object"};
}
double const& json::get_number() const {
    if (is_number()) return pimpl->num;
    else throw json_exception{"get_number() const called on a non number json object"};
}
bool& json::get_bool() {
    if (is_bool()) return pimpl->boolean;
    else throw json_exception{"get_bool() called on a non bool json object"};
}
bool const& json::get_bool() const {
    if (is_bool()) return pimpl->boolean;
    else throw json_exception{"get_bool() const called on a non bool json object"};
}
std::string& json::get_string() {
    if (is_string()) return pimpl->string;
    else throw json_exception{"get_string() called on a non string json object"};
}
std::string const& json::get_string() const {
    if (is_string()) return pimpl->string;
    else throw json_exception{"get_string() const called on a non string json object"};
}

void json::set_string(std::string const& s) {set_null(); pimpl->string = s;}
void json::set_bool(bool b) {set_null(); pimpl->boolean = b; pimpl->type = 'b';}
void json::set_number(double n) {set_null(); pimpl->num = n;}
void json::set_list() {set_null(); pimpl->type = '[';}
void json::set_dictionary() {set_null(); pimpl->type = '{';}
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
        throw json_exception{"push_front() called on a non list json object"};
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
        throw json_exception{"push_back() called on a non list json object"};
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
        throw json_exception{"insert() called on a non dictionary json object"};
    }
}

// -------------------- OPERATORS OVERLOAD --------------------
json& json::operator=(json const& rhs) { // COPY ASSIGNMENT
    if (this != &rhs) {
        pimpl->type = rhs.pimpl->type;
        if (rhs.is_number()) {
            set_number(rhs.get_number());
        } else if (rhs.is_bool()) {
            set_bool(rhs.get_bool());
        } else if (rhs.is_string()) {
            set_string(rhs.get_string());
        } else if (rhs.is_list()) {
            set_null();
            set_list();
            auto ptr = rhs.pimpl->l_front;
            while (ptr) {
                push_back(ptr->info);
                ptr = ptr->next;
            }
        } else if (rhs.is_dictionary()) {
            set_null();
            set_dictionary();
            auto ptr = rhs.pimpl->d_front;
            while (ptr) {
                insert(ptr->info);
                ptr = ptr->next;
            }
        }
    }
    return *this;
}
json& json::operator=(json&& rhs) { // MOVE ASSIGNMENT
    if (this != &rhs) {
        set_null();
        pimpl->type = rhs.pimpl->type;
        pimpl->num = rhs.pimpl->num;
        pimpl->boolean = rhs.pimpl->boolean;
        pimpl->string = rhs.pimpl->string;

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
        throw json_exception{"Key not found using operator[] const, cannot insert a new element into the dictionary"};
    } else {
        throw json_exception{"Operator[] const called on a non dictionary json object"};
    }
}
json& json::operator[](std::string const& s) {
    if (is_dictionary()) {
        auto ptr = pimpl->d_front;
        while (ptr) {
            if (ptr->info.first == s) {
                return ptr->info.second;
            }
            ptr = ptr->next;
        }
        if (pimpl->d_front == nullptr) {
            pimpl->d_front = pimpl->d_back = new impl::Dictionary_cell{{s, json{}}, nullptr};
        } else {
            pimpl->d_back->next = new impl::Dictionary_cell{{s, json{}}, nullptr};
            pimpl->d_back = pimpl->d_back->next;
        }
        return pimpl->d_back->info.second;
    } else {
        throw json_exception{"Operator[] called on a non dictionary json object"};
    }
}

// -------------------- CONTEXT-FREE GRAMMAR --------------------
json J(std::istream&);
std::string parse_string(std::istream& is) {
    char c;
    std::string string;
    is.get(c);
    while (c != '"') {
        if (is.eof()) {
            throw json_exception{"reached eof while parsing a string without finding closing \""};
        }
        if (c == '\\') {
            string.push_back(c);
            is.get(c);
            if (c == '"') {
                string.push_back(c);
                is.get(c);
            }
        } else {
            string.push_back(c);
            is.get(c);
        }
    }
    return string;
}

json L(std::istream& is) {
    char c;
    json j;
    j.set_list();
    do {
        j.push_back(J(is));
        is >> c;
    } while (c == ',');
    is.putback(c);
    return j;
}

json D(std::istream& is) {
    char c;
    json j, value;
    std::string key;
    j.set_dictionary();
    do {
        is >> c; // leggo "
        if (c != '"') {
            std::string error = "Expected a string key value while parsing a dictionary, found: ";
            error += c;
            throw json_exception{error};
        } else {
            key = parse_string(is); // leggo 'key'
            is >> c;
            if (c != ':') {
                std::string error = "Expected ':' while parsing a pair, found: ";
                error += c;
                throw json_exception{error};
            }
            value = J(is); // leggo 'value'
            std::pair<std::string, json> pair{key, value};
            j.insert(pair);
            is >> c;
        }
    } while (c == ',');
    is.putback(c);
    return j;
}

json J(std::istream& is) {
    json j;
    char c;
    is >> c; // salto gli spazi e leggo

    if ((c >= '0' and c <= '9') or c == '-') { // num
        double num;
        is.putback(c);
        is >> num;
        j.set_number(num);
        return j;

    } else if (c == 't'){ // bool->true
        std::string target = "true";
        for (uint i = 1; i < target.length(); i++) {
            is >> c;
            if (c != target.at(i)) {
                std::string error = "";
                error += target.at(i);
                error += " expected while parsing a boolean value, found: ";
                error += c;
                throw json_exception{error};
            }
        }
        j.set_bool(true);
        return j;

    } else if (c == 'f') { // bool->false
        std::string target = "false";
        for (uint i = 1; i < target.length(); i++) {
            is >> c;
            if (c != target.at(i)) {
                std::string error = "";
                error += target.at(i);
                error += " expected while parsing a boolean value, found: ";
                error += c;
                throw json_exception{error};
            }
        }
        j.set_bool(false);
        return j;

    } else if (c == 'n') { // null
        std::string target = "null";
        for (uint i = 1; i < target.length(); i++) {
            is >> c;
            if (c != target.at(i)) {
                std::string error = "";
                error += target.at(i);
                error += " expected while parsing a null value, found: ";
                error += c;
                throw json_exception{error};
            }
        }
        j.set_null();
        return j;

    } else if (c == '"') { // string
        j.set_string(parse_string(is));
        return j;

    } else if (c == '[') {
        is >> c;
        if (c != ']') {
            is.putback(c);
            j = L(is);
            is >> c;
            if (c != ']') {
                std::string error = "closing ] expected while parsing a list, found: ";
                error += c;
                throw json_exception{error};
            }
        } else { // lista vuota
            j.set_list();
        }
        return j;

    } else if (c == '{') {
        is >> c;
        if (c != '}') {
            is.putback(c);
            j = D(is);
            is >> c;
            if (c != '}') {
                std::string error = "closing } expected while parsing a dictionary, found: ";
                error += c;
                throw json_exception{error};
            }
        } else { // dizionario vuoto
            j.set_dictionary();
        }
        return j;

    } else {
        is.putback(c);
        std::string error = "unknown symbol: ";
        error += c;
        throw json_exception{error};
    }
}

// -------------------- I/O STREAMS OPERATORS OVERLOAD --------------------
std::ostream& json_print(std::ostream& os, json const& j) {
    if (j.is_number()) os << j.get_number();
    if (j.is_bool()) os << std::boolalpha << j.get_bool();
    if (j.is_string()) os << '"' << j.get_string() << '"';
    if (j.is_null()) os << "null";
    if (j.is_list()) {
        os << "[";
        auto it = j.begin_list();
        if (it != j.end_list()) { // la prima volta non stampo la virgola
            json_print(os, *it);
            it++;
        }
        while (it != j.end_list()) { // partendo da un eventuale secondo elemento stampo prima la virgola e poi l'oggetto
            os << ", ";
            json_print(os, *it);
            it++;
        }
        os << "]";
    }
    if (j.is_dictionary()) {
        os << "{";
        auto it = j.begin_dictionary();
        if (it != j.end_dictionary()) { // la prima volta non stampo la virgola
            os << '"' << it->first << "\" : ";
            json_print(os, it->second);
            it++;
        }
        while (it != j.end_dictionary()) { // partendo da un eventuale secondo elemento stampo prima la virgola e poi l'oggetto
            os << ", ";
            os << '"' << it->first << "\" : ";
            json_print(os, it->second);
            it++;
        }
        os << "}";

    }
    return os;
}

std::istream& operator>>(std::istream& lhs, json& rhs) {
    char c;
    lhs >> c;
    if (lhs.eof()) { // json vuoto
        json j;
        rhs = j;
        return lhs;
    } else {
        lhs.putback(c);
        rhs = J(lhs);
        lhs >> c; // mangio l'ultimo carattere newline
        if (lhs.eof()) return lhs;
        else {
            lhs.putback(c);
            std::string error = "istream not empty after parsing\n----- istream dump -----\n";
            while (!lhs.eof()) error += char(lhs.get());
            error += "------------------------";
            throw json_exception{error};
        }
    }
}

std::ostream& operator<<(std::ostream& lhs, json const& rhs) {
    lhs << std::endl << "....................." << std::endl;
    json_print(lhs, rhs);
    lhs << std::endl << "....................." << std::endl;
    return lhs;
}
