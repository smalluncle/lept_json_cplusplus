#pragma once

#ifdef WIN32
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif	

#include <string>
#include <cassert>
#include <cmath>
#include <codecvt>
#include <sstream>
#include <iomanip>

#include "LeptValue.h"


/**
* 本类为解析Json类，提供解析、生成接口。
*/


class LeptJson
{
	
private:
	enum class State {
		ZEOR,ONE,TWO,THREE,FOUR,FIVE,SIX,SEVEN,EIGHT
	};

private:
	const char* json;

private:
	void lept_parse_whitespace();

	Expect lept_parse_value(LeptValue& lv);
	Expect lept_parse_literal(LeptValue& lv, const char* literal, LeptType type);
	Expect lept_parse_number(LeptValue& lv);
	Expect lept_parse_string_raw(std::string& str);
	Expect lept_parse_string(LeptValue& lv);
	Expect lept_parse_array(LeptValue& lv);
	Expect lept_parse_object(LeptValue& lv);


	const char* lept_parse_hex4(unsigned& u) ;
	void lept_encode_utf8(std::string& strBuf, const unsigned& u);

	bool checkNextChar() {
		if (*json != ' ' && *json != '\t' && *json != '\n' && *json != '\r' &&
			*json != '\0') {
			return false;
		}
		return true;
	}
	void EXPECT(char ch) {
		assert(*json == ch); ++json;
	}

	Expect STRING_ERROR(Expect error) {
		return error;
	}

	std::string hexTostr(unsigned short code) const;
	void lept_stringify_string(std::string& jsonStr, const std::string& s) const;


public:
	LeptJson(const char *js) : json(js) {};
	LeptJson(const LeptJson&) = delete;
	LeptJson(const LeptJson&&) = delete;
	Expect lept_parse(LeptValue& v);
	std::string lept_stringify(const LeptValue& v) const;
	void lept_copy(LeptValue& dst, const LeptValue& src) const;
	void lept_move(LeptValue& dst, LeptValue& src) const;
	void lept_swap(LeptValue& dst, LeptValue& src) const;
	void reset_json(const char* json) {
		this->json = json;
	}
};

inline
Expect LeptJson::lept_parse_literal(LeptValue& lv, const char* lit, LeptType type) {
	assert(lit != nullptr);
	EXPECT(*lit);
	++lit;
	while ((*lit) != '\0') {
		if (*(this->json) != *lit) {
			return Expect::LEPT_PARSE_INVALID_VALUE;
		}
		++this->json;
		++lit;
	}
	lv.setType(type);
	return Expect::LEPT_PARSE_OK;
}


inline
Expect LeptJson::lept_parse_number(LeptValue& lv) {
	/* \TODO validate number */
	/* DFA */
	State state = State::ZEOR;
	const char* tempJson = json;
	bool isEnd = false;
	while (*tempJson != ' ' && *tempJson != '\0' && !isEnd) {
		char nextChar = *tempJson;
		++tempJson;
		switch (state)
		{
		case LeptJson::State::ZEOR:
			if(nextChar == '-') state = State::ONE;
			else if(nextChar == '0') state = State::TWO;
			else if(nextChar >= '1' && nextChar <= '9') state = State::THREE;
			else return Expect::LEPT_PARSE_INVALID_VALUE;
			break;
		case LeptJson::State::ONE:
			if (nextChar == '0') state = State::TWO;
			else if (nextChar >= '1' && nextChar <= '9') state = State::THREE;
			else return Expect::LEPT_PARSE_INVALID_VALUE;
			break;
		case LeptJson::State::TWO:
			if (nextChar == '.') state = State::FOUR;
			else if (nextChar == 'E' || nextChar == 'e') state = State::FIVE;
			else { isEnd = true; --tempJson; }
			break;
		case LeptJson::State::THREE:
			if (nextChar == '.') state = State::FOUR;
			else if (nextChar >= '0' && nextChar <= '9') state = State::THREE;
			else if (nextChar == 'E' || nextChar == 'e') state = State::FIVE;
			else { isEnd = true; --tempJson; }
			break;
		case LeptJson::State::FOUR:
			if (nextChar >= '0' && nextChar <= '9') state = State::SIX;
			else return Expect::LEPT_PARSE_INVALID_VALUE;
			break;
		case LeptJson::State::FIVE:
			if (nextChar >= '0' && nextChar <= '9') state = State::EIGHT;
			else if (nextChar == '+' || nextChar == '-') state = State::SEVEN;
			else return Expect::LEPT_PARSE_INVALID_VALUE;
			break;
		case LeptJson::State::SIX:
			if (nextChar >= '0' && nextChar <= '9') state = State::SIX;
			else if (nextChar == 'E' || nextChar == 'e') state = State::FIVE;
			else { isEnd = true; --tempJson; }
			break;
		case LeptJson::State::SEVEN:
			if (nextChar >= '0' && nextChar <= '9') state = State::EIGHT;
			else return Expect::LEPT_PARSE_INVALID_VALUE;
			break;
		case LeptJson::State::EIGHT:
			if (nextChar >= '0' && nextChar <= '9') state = State::EIGHT;
			else { isEnd = true; --tempJson; }
			break;
		default:
			return Expect::LEPT_PARSE_INVALID_VALUE;
			break;
		}
	}

	if (state != State::TWO && state != State::THREE && state != State::SIX
		&& state != State::EIGHT) {
		return Expect::LEPT_PARSE_INVALID_VALUE;
	}

	
	double val = strtod(this->json, nullptr);

	this->json = tempJson;

	if (val == HUGE_VALF || val == HUGE_VAL || val == HUGE_VALL || val == -INFINITY) {
		return Expect::LEPT_PARSE_NUMBER_TOO_BIG;
	}
	lv.lept_set_number(val);
	return Expect::LEPT_PARSE_OK;
}

inline 
Expect LeptJson::lept_parse_string(LeptValue& lv) {
	std::string str;
	auto ret = lept_parse_string_raw(str);
	if (ret == Expect::LEPT_PARSE_OK) {
		lv.lept_set_string(str);
	}
	return ret;
}

inline 
Expect LeptJson::lept_parse_string_raw(std::string& str) {
	std::string strBuf;
	EXPECT('\"');
	char nextCh = 0;
	unsigned u = 0;
	unsigned codePoint = 0;
	while (1) { 
		char ch = *json++;
		switch (ch)
		{
		case '\"':
			str = strBuf;
			strBuf.clear();
			return Expect::LEPT_PARSE_OK;
		case '\0':
			strBuf.clear();
			return Expect::LEPT_PARSE_MISS_QUOTATION_MARK;
		case '\\':
			nextCh = *json++;
			switch (nextCh)
			{
			case '\"': strBuf.push_back('\"'); break;
			case '/': strBuf.push_back('/'); break;
			case '\\': strBuf.push_back('\\'); break;
			case 'b': strBuf.push_back('\b'); break;
			case 'f': strBuf.push_back('\f'); break;
			case 'n': strBuf.push_back('\n'); break;
			case 'r': strBuf.push_back('\r'); break;
			case 't': strBuf.push_back('\t'); break;
			case 'u':
				// unicode
				if (!(json = lept_parse_hex4(u)))
					return STRING_ERROR(Expect::LEPT_PARSE_INVALID_UNICODE_HEX);
				/* \TODO surrogate handling */
				codePoint = u;
				if (u >= 0x8d00 && u <= 0xDBFF) {
					codePoint = 0x10000 + (u - 0xD800) * 0x400;
					if (*json++ == '\\') {
						if (*json++ == 'u') {
							if (!(json = lept_parse_hex4(u)))
								return STRING_ERROR(Expect::LEPT_PARSE_INVALID_UNICODE_HEX);
							if (u >= 0xDC00 && u <= 0xDFFF) {
								codePoint += (u - 0xDC00);
							}
							else {
								return STRING_ERROR(Expect::LEPT_PARSE_INVALID_UNICODE_SURROGATE);
							}
						}
						else {
							return STRING_ERROR(Expect::LEPT_PARSE_INVALID_UNICODE_SURROGATE);
						}
					}
					else {
						return STRING_ERROR(Expect::LEPT_PARSE_INVALID_UNICODE_SURROGATE);
					}
				}
				u = codePoint;
				lept_encode_utf8(strBuf,u);
				break;
			default:
				return Expect::LEPT_PARSE_INVALID_STRING_ESCAPE;
				break;
			}
			break;
		default:
			if (ch >= 0 && ch <= 31) {
				return Expect::LEPT_PARSE_INVALID_STRING_CHAR;
			}
			strBuf.push_back(ch);
		}
	}
	return Expect();
}


inline
void LeptJson::lept_parse_whitespace() {
	while (*json == ' ' || *json == '\t' || *json == '\n' || *json == '\r')
		json++;
}

inline
Expect LeptJson::lept_parse_value(LeptValue& lv) {
	
	switch (*this->json)
	{
	case 'n': return lept_parse_literal(lv,"null",LeptType::LEPT_NULL);
	case 't': return lept_parse_literal(lv, "true", LeptType::LEPT_TRUE);
	case 'f': return lept_parse_literal(lv, "false", LeptType::LEPT_FALSE);
	case '\"': return lept_parse_string(lv);
	case '[': return lept_parse_array(lv);
	case '{': return lept_parse_object(lv);
	case '\0': return Expect::LEPT_PARSE_EXPECT_VALUE;
	default:
		return lept_parse_number(lv);
		break;
	}
}


inline 
Expect LeptJson::lept_parse(LeptValue& v) {
	v.lept_set_null();

	// 消除前置空格
	lept_parse_whitespace();

	auto ret = lept_parse_value(v);
	if (ret == Expect::LEPT_PARSE_OK) {
		lept_parse_whitespace();
		if (*this->json != '\0') {
			v.lept_set_null();
			ret = Expect::LEPT_PARSE_ROOT_NOT_SINGULAR;
		}
	}

	return ret;
}

inline 
const char* LeptJson::lept_parse_hex4(unsigned& u) {
	u = 0;

	for (int i = 0; i < 4; ++i) {
		char ch = *json++;
		u <<= 4;
		if (ch >= '0' && ch <= '9')  u |= ch - '0';
		else if (ch >= 'A' && ch <= 'F')  u |= ch - ('A' - 10);
		else if (ch >= 'a' && ch <= 'f')  u |= ch - ('a' - 10);
		else return NULL;
	}
	return json;
}

inline
void LeptJson::lept_encode_utf8(std::string& strBuf,const unsigned& u) {
	std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> utf8_ucs4_cvt;
	std::string u8_str_from_ucs4 = utf8_ucs4_cvt.to_bytes(u);
	strBuf += u8_str_from_ucs4;
}

inline 
Expect LeptJson::lept_parse_array(LeptValue& lv) {
	EXPECT('[');
	std::vector<LeptValue> arrBuf;
	lept_parse_whitespace();
	if (*json == ']') {
		++json;
		lv.lept_set_array({});
		return Expect::LEPT_PARSE_OK;
	}
	while (1) {
		lept_parse_whitespace();
		LeptValue tempLv;
		auto ret = lept_parse_value(tempLv);
		if (ret != Expect::LEPT_PARSE_OK) {
			return ret;
		}
		arrBuf.push_back(tempLv);
		lept_parse_whitespace();
		if (*json == ',')
			json++;
		else if (*json == ']') {
			json++;
			lv.lept_set_array(arrBuf);
			return Expect::LEPT_PARSE_OK;
		}
		else {
			return Expect::LEPT_PARSE_MISS_COMMA_OR_SQUARE_BRACKET;
		}
	}
}

inline 
Expect LeptJson::lept_parse_object(LeptValue& lv) {
	EXPECT('{');
	std::vector<std::pair<std::string, LeptValue>> objectBuf;
	lept_parse_whitespace();
	if (*json == '}') {
		++json;
		lv.lept_set_object({});
		return Expect::LEPT_PARSE_OK;
	}
	while (1) {
		std::pair<std::string, LeptValue> member;
		lept_parse_whitespace();
		if (*json != '\"') {
			return Expect::LEPT_PARSE_MISS_KEY;
		}
		std::string m_key;
		LeptValue m_value;
		auto ret = lept_parse_string_raw(m_key);
		if (ret != Expect::LEPT_PARSE_OK) {
			return ret;
		}
		lept_parse_whitespace();
		if (*json != ':') {
			return Expect::LEPT_PARSE_MISS_COLON;
		}
		++json;
		lept_parse_whitespace();
		ret = lept_parse_value(m_value);
		if (ret != Expect::LEPT_PARSE_OK) {
			return ret;
		}
		member = std::make_pair(m_key, m_value);
		objectBuf.push_back(member);
		lept_parse_whitespace();
		
		if (*json == ',')
			json++;
		else if (*json == '}') {
			json++;
			lv.lept_set_object(objectBuf);
			return Expect::LEPT_PARSE_OK;
		}
		else {
			return Expect::LEPT_PARSE_MISS_COMMA_OR_CURLY_BRACKET;
		}
	}
}


inline
std::string LeptJson::hexTostr(unsigned short code) const {
	std::string str = "++++";
	for (int i = 3; i >= 0; --i, code >>= 4) {
		if ((code & 0xf) <= 9)
			str[i] = (code & 0xf) + '0';
		else
			str[i] = (code & 0xf) + 'A' - 0x0a;
	}
	return str;
}

inline
void LeptJson::lept_stringify_string(std::string& jsonStr,const std::string& s) const {

	jsonStr.push_back('\"');
	unsigned char ch = 0;
	unsigned short code = 0;
	unsigned int codePoint = 0;
	unsigned short highPoint = 0;
	unsigned short lowPoint = 0;
	size_t len = s.size();
	for (size_t i = 0; i < len; ++i) {
		ch = s[i];
		code = 0;
		codePoint = 0;
		highPoint = 0;
		lowPoint = 0;

		codePoint = 0;
		if (ch == 0) {
			code |= ch;
		}
		else if (ch > 0 && ch <= 0x20) {
			switch (ch)
			{
			case '\b': jsonStr += std::string("\\b"); break;
			case '\f': jsonStr += std::string("\\f"); break;
			case '\n': jsonStr += std::string("\\n"); break;
			case '\r': jsonStr += std::string("\\r"); break;
			case '\t': jsonStr += std::string("\\t"); break;
			case ' ':  jsonStr += std::string(" "); break;
			default:
				break;
			}
			continue;
		}
		else if (ch > 0x20 && ch <= 0x7f) {
			if (ch == '\"' || ch == '\\') {
				jsonStr.push_back('\\');
			}
			jsonStr.push_back(ch);
			continue;
		}
		else if (ch >= 0xc0 && ch <= 0xdf) {
			code |= ((unsigned short)(ch & 0x1f) << 6);
			ch = s[++i];
			code |= (ch & 0b00111111);
		}
		else if (ch >= 0xe0 && ch <= 0xef) {
			code |= ((unsigned short)(ch & 0x0f) << 12);
			ch = s[++i];
			code |= ((unsigned short)(ch & 0b00111111) << 6);
			ch = s[++i];
			code |= (ch & 0b00111111);
		}
		else if (ch >= 0xf0 && ch <= 0xf7) {
			codePoint |= ((unsigned int)(ch & 0x00000111) << 18);
			ch = s[++i];
			codePoint |= ((unsigned int)(ch & 0b00111111) << 12);
			ch = s[++i];
			codePoint |= ((unsigned int)(ch & 0b00111111) << 6);
			ch = s[++i];
			codePoint |= (ch & 0b00111111);
			highPoint = (((codePoint - 0x10000) & 0xf000) / 0x400) + 0xd800;
			lowPoint = ((codePoint - 0x10000) & 0x0fff) + 0xdc00;

			jsonStr += std::string("\\u");
			jsonStr += hexTostr(highPoint);

			jsonStr += std::string("\\u");
			jsonStr += hexTostr(lowPoint);
			continue;
		}
		jsonStr += std::string("\\u");
		jsonStr += hexTostr(code);
	}
	jsonStr.push_back('\"');
}

inline
std::string LeptJson::lept_stringify(const LeptValue& v) const {
	size_t i;
	std::string stringify_json;
	switch (v.lept_get_type()) {
	case LeptType::LEPT_NULL:   stringify_json = "null";  break;
	case LeptType::LEPT_FALSE:  stringify_json = "false";  break;
	case LeptType::LEPT_TRUE:   stringify_json = "true";  break;
	case LeptType::LEPT_NUMBER: {
		std::stringstream ss;
		ss << std::setprecision(17) << v.lept_get_number();
		stringify_json = ss.str();  
		break;
	}
	case LeptType::LEPT_STRING: {
		lept_stringify_string(stringify_json,v.lept_get_string());
		break;
	}
	case LeptType::LEPT_ARRAY:
		stringify_json.push_back('[');
		for (i = 0; i < v.lept_get_array_size(); ++i) {
			if (i > 0) {
				stringify_json.push_back(',');
			}
			stringify_json += lept_stringify(v.lept_get_array_element(i));
		}
		stringify_json.push_back(']');
		break;
	case LeptType::LEPT_OBJECT:
		stringify_json.push_back('{');
		for (i = 0; i < v.lept_get_object_size(); ++i) {
			if(i > 0) 
				stringify_json.push_back(',');
			lept_stringify_string(stringify_json,v.lept_get_object_key(i));
			stringify_json.push_back(':');
			stringify_json += lept_stringify(v.lept_get_object_member_value(i));
		}
		stringify_json.push_back('}');
		break;
	default: assert(0 && "invalid type");
	}
	return stringify_json;

}

inline
void LeptJson::lept_copy(LeptValue& dst, const LeptValue& src) const {
	dst = src;
}

inline
void LeptJson::lept_move(LeptValue& dst, LeptValue& src) const {
	dst = std::move(src);
	src.clearAll();
}

inline 
void LeptJson::lept_swap(LeptValue& dst, LeptValue& src) const {
	LeptValue temp = src;
	src = dst;
	dst = temp;
}







