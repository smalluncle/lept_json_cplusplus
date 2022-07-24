#pragma once

#include <iostream>
#include <cassert>
#include <vector>
#include <map>
#include <string>
#include "EnumClass.h"
/*
* 本类为树结点类，解析Json字符串将生成该树。
*/

using std::string;
const size_t LEPT_KEY_NOT_EXIST = ((size_t)-1);

class LeptValue
{

private:
	std::vector<LeptValue> arr;
	std::vector<std::pair<std::string, LeptValue>> object;
	std::string str;
	double number;
	LeptType type;
public:

	LeptValue() : str(""), number(0), type(LeptType::LEPT_NULL) {  };
	
	void setType(LeptType tp) {
		this->type = tp;
	}
	void setNumber(double number) {
		this->number = number;
	}

	bool lept_get_boolean() {
		assert(this->type == LeptType::LEPT_TRUE || this->type == LeptType::LEPT_FALSE);
		return (this->type == LeptType::LEPT_TRUE) ? true : false;
	}
	void lept_set_boolean(bool b) {
		this->type = b ? LeptType::LEPT_TRUE : LeptType::LEPT_FALSE;
	}


	void lept_set_null();
	LeptType lept_get_type() const;
	void lept_set_number(double n);
	double lept_get_number() const;
	// array
	void lept_set_string(const string& str);
	void lept_set_array(const std::vector<LeptValue>& arr);
	void lept_set_array_capacity(const std::vector<LeptValue>& arr, size_t capacity);
	size_t lept_get_array_capacity() const;
	size_t lept_get_array_size() const;
	LeptValue lept_get_array_element(size_t index) const;
	void lept_pushback_array_element(const LeptValue& element);
	void lept_popback_array_element();
	void lept_erase_array_element(size_t _start,size_t _count);
	void lept_insert_array_element(const LeptValue& e, size_t index);
	void lept_shrink_array();
	void lept_clear_array();

	// string
	string lept_get_string() const;
	size_t lept_get_string_length() const;
	

	// object
	size_t lept_get_object_size() const;
	void lept_set_object(const std::vector<std::pair<std::string, LeptValue>>& objectArr);
	std::string lept_get_object_key(size_t index) const;
	size_t lept_get_object_key_length(size_t index) const;
	LeptValue lept_get_object_member_value(size_t index) const;
	size_t lept_find_object_member_index(const std::string& key) const;
	void lept_set_object_member(std::pair<std::string, LeptValue> _member);
	void lept_remove_object_member(size_t index);
	void lept_shrink_object();
	size_t lept_get_object_capacity() const;
	void lept_clear_object();



	void clearAll();
};

inline 
void LeptValue::lept_set_number(double n){ 
	setNumber(n); 
	this->type = LeptType::LEPT_NUMBER; 
	this->str.clear();
}

inline 
void  LeptValue::lept_set_null() {
	this->str.clear();
	this->arr.clear();
	this->number = 0.0;
	this->object.clear();
	this->type = LeptType::LEPT_NULL;
}

inline 
LeptType LeptValue::lept_get_type() const {
	return this->type;
}

inline 
double LeptValue::lept_get_number() const {
	assert(this->type == LeptType::LEPT_NUMBER);
	return this->number;
}

inline 
void LeptValue::lept_set_string(const string& str) {
	this->str = str; 
	this->type = LeptType::LEPT_STRING; 
	this->number = 0.0;
}

inline
string LeptValue::lept_get_string() const {
	assert(this->type == LeptType::LEPT_STRING);
	return this->str;
}

inline 
size_t LeptValue::lept_get_string_length() const {
	assert(this->type == LeptType::LEPT_STRING);
	return this->str.size();
}

inline
size_t LeptValue::lept_get_array_size() const {
	assert(this->type == LeptType::LEPT_ARRAY);
	return this->arr.size();
}

inline
size_t LeptValue::lept_get_array_capacity() const {
	assert(this->type == LeptType::LEPT_ARRAY);
	return this->arr.capacity();
}

inline
void LeptValue::lept_set_array(const std::vector<LeptValue>& arr) {
	this->arr = arr;
	this->type = LeptType::LEPT_ARRAY;
	this->number = 0.0;
	this->str = "";
}

inline
void LeptValue::lept_set_array_capacity(const std::vector<LeptValue>& arr, size_t capacity) {
	lept_set_array(arr);
	this->arr.reserve(capacity);
}

inline 
LeptValue LeptValue::lept_get_array_element(size_t index) const {
	assert(this->type == LeptType::LEPT_ARRAY);
	assert(index < this->arr.size());
	return this->arr[index];
}

inline
void LeptValue::lept_pushback_array_element(const LeptValue& element) {
	assert(this->type == LeptType::LEPT_ARRAY);
	this->arr.push_back(element);
}

inline
void LeptValue::lept_popback_array_element() {
	assert(this->type == LeptType::LEPT_ARRAY);
	this->arr.pop_back();
}

inline
void LeptValue::lept_insert_array_element(const LeptValue& e,size_t index) {
	assert(this->type == LeptType::LEPT_ARRAY && index <= this->arr.size());
	auto it = this->arr.begin() + index;
	this->arr.insert(it, e);
}

inline
void LeptValue::lept_erase_array_element(size_t _start, size_t _count) {
	assert(this->type == LeptType::LEPT_ARRAY && _start + _count <= this->arr.size());
	size_t num = 0;
	auto it = this->arr.begin() + _start;
	while (num < _count) {
		it = this->arr.erase(it);
		++num;


	}
}

inline
void LeptValue::lept_shrink_array() {
	assert(this->type == LeptType::LEPT_ARRAY);
	this->arr.shrink_to_fit();
}

inline
void LeptValue::lept_clear_array() {
	assert(this->type == LeptType::LEPT_ARRAY);
	this->arr.clear();
}

inline
void LeptValue::clearAll() {
	this->arr.clear();
	this->str.clear();
	this->number = 0.0;
	this->object.clear();
	this->type = LeptType::LEPT_NULL;
}

inline 
size_t LeptValue::lept_get_object_size() const {
	assert(this->type == LeptType::LEPT_OBJECT);
	return this->object.size();
}

inline 
void LeptValue::lept_set_object(const std::vector<std::pair<std::string,LeptValue>>& objectArr){
	this->type = LeptType::LEPT_OBJECT;
	this->object = objectArr;
}

inline
std::string LeptValue::lept_get_object_key(size_t index)const {
	assert(this->type == LeptType::LEPT_OBJECT);
	return this->object[index].first;
}

inline
size_t LeptValue::lept_get_object_key_length(size_t index)const {
	assert(this->type == LeptType::LEPT_OBJECT);
	return this->object[index].first.size();
}

inline
LeptValue LeptValue::lept_get_object_member_value(size_t index) const {
	assert(this->type == LeptType::LEPT_OBJECT);
	return this->object[index].second;
}

inline
size_t LeptValue::lept_find_object_member_index(const std::string& key) const {
	assert(this->type == LeptType::LEPT_OBJECT);
	size_t keySize = key.size();
	for (size_t i = 0; i < this->object.size(); i++) {
		if (this->object[i].first.size() == keySize && memcmp(this->object[i].first.data(), key.data(), keySize) == 0) {
			return i;
		}
	}
	return LEPT_KEY_NOT_EXIST;
}


inline
void LeptValue::lept_set_object_member(std::pair<std::string, LeptValue> _member) {
	assert(this->type == LeptType::LEPT_OBJECT);
	this->object.push_back(_member);
}

inline
void LeptValue::lept_remove_object_member(size_t index) {
	assert(this->type == LeptType::LEPT_OBJECT && index < this->object.size());
	this->object.erase(this->object.begin() + index);
}

inline
void LeptValue::lept_shrink_object() {
	assert(this->type == LeptType::LEPT_OBJECT);
	this->object.shrink_to_fit();
}

inline
size_t LeptValue::lept_get_object_capacity() const {
	assert(this->type == LeptType::LEPT_OBJECT);
	return this->object.capacity();
}

inline
void LeptValue::lept_clear_object() {
	assert(this->type == LeptType::LEPT_OBJECT);
	this->object.clear();
}



