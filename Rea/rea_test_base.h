#pragma once

#include <iostream>

#define TEST_SLOT_MAP 
//#define TEST_CONTROLLED_SLOT_MAP 
//#define TEST_VERSIONED_SLOT_MAP 
//#define TEST_REGULATED_SLOT_MAP 
//#define TEST_DENSE_MAP 
//#define TEST_VERSIONED_DENSE_MAP 

#if defined(TEST_DENSE_MAP) || defined(TEST_VERSIONED_DENSE_MAP) 
#define TEST_DENSE
#else
#define TEST_SLOT
#endif


template<typename I>
int get_id_index(I id) {
	return id;
}

template<typename I>
int get_id_version(I id) {
	return 0;
}

template<typename S, typename V>
int get_id_index(std::pair<S, V> id) {
	return id.first;
}

template<typename S, typename V>
int get_id_version(std::pair<S, V> id) {
	return id.second;
}

struct instrumented {
	explicit instrumented(int value) : value(value) {}

	instrumented() {
		std::cout << "default constructor" << std::endl;
	}

	~instrumented() {
		std::cout << "destructor" << std::endl;
	}

	instrumented(const instrumented& x) {
		value = x.value;
		std::cout << "copy constructor" << std::endl;
	}

	instrumented(instrumented&& x) {
		value = x.value;
		std::cout << "move constructor" << std::endl;
	}

	instrumented& operator=(const instrumented& x) {
		value = x.value;
		std::cout << "copy assignement" << std::endl;
		return *this;
	}

	instrumented& operator=(instrumented&& x) {
		value = x.value;
		std::cout << "move assignement" << std::endl;
		return *this;
	}

	int value;
};
