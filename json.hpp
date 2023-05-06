#include <iostream>
#include <string>
#include <limits>
#include <assert.h>
#include <fstream>

struct json_exception {
	std::string msg;
};

class json {

public:
	
	struct list_iterator;
	struct dictionary_iterator;
	struct const_list_iterator;
	struct const_dictionary_iterator;
	
	json(); // ok
	json(json const&); // ok
	json(json&&); // ok
	~json(); // ok
	
	json& operator=(json const&); // ok
	json& operator=(json&&); // ok

	bool is_list() const; // ok
	bool is_dictionary() const; // ok
	bool is_string() const; // ok
	bool is_number() const; // ok
	bool is_bool() const; // ok
	bool is_null() const; // ok

	json const& operator[](std::string const&) const;
	json& operator[](std::string const&);

	list_iterator begin_list(); // ?
	const_list_iterator begin_list() const; // ?
	list_iterator end_list(); // ?
	const_list_iterator end_list() const; // ?

	dictionary_iterator begin_dictionary(); // ?
	const_dictionary_iterator begin_dictionary() const; // ?
	dictionary_iterator end_dictionary(); // ?
	const_dictionary_iterator end_dictionary() const; // ?

	double& get_number(); // ok
	double const& get_number() const; // ok

	bool& get_bool(); // ok
	bool const& get_bool() const; // ok

	std::string& get_string(); // ok
	std::string const& get_string() const; // ok

	void set_string(std::string const&); // ok
	void set_bool(bool); // ok
	void set_number(double); // ok
	void set_null(); // ok
	void set_list(); // ok
	void set_dictionary(); // ok
	void push_front(json const&); // ok
	void push_back(json const&); // ok
	void insert(std::pair<std::string,json> const&); // ok

private:
		
	struct impl;
	impl* pimpl;
	
};

std::ostream& operator<<(std::ostream& lhs, json const& rhs);
std::istream& operator>>(std::istream& lhs, json& rhs);
