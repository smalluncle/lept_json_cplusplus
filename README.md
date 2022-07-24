# lept_json_cplusplus

本项目是在开源项目json-tutorial（项目地址https://github.com/miloyip/json-tutorial）的基础上进行改写的。 原开源项目是用C开发的，本项目用C++进行实现。具体教程在原项目中，项目地址如上。

下面介绍本项目的一些事项。

### 可开发环境

1、vs2022

2、C++11

### 相关文件说明

test.cpp 测试文件

EnumClass.h 枚举类文件，内含两个枚举类Expect\LeptType,其中前者是期待函数返回的值，后者是json的基本数据类型。

LeptValue.h 内含一个LeptValue类，表示树结点，将json字符串解析后将生成该类的一个对象，通过这个对象可以做一下操作。

LeptJson.h 内含一个LeptJson类型，通过该类提供两个主要接口：lept_parse(LeptValue& v)解析json字符串生成LeptValue对象。std::string lept_stringify(const LeptValue& v)，通过LeptValue对象生成json字符串。



### 使用说明

```c++
    LeptValue v; // 创建LeptValue对象
    LeptJson lj(json.c_str());// 创建LeptJson对象，需要传入被解析的json字符串
// lj.lept_parse(v) 解析json字符串，并传入LeptValue对象。
    EXPECT_EQ_INT(Expect::LEPT_PARSE_OK, lj.lept_parse(v), _line_);
    EXPECT_EQ_INT(LeptType::LEPT_STRING, v.lept_get_type(), _line_);
    EXPECT_EQ_STRING(expect, v.lept_get_string(), v.lept_get_string_length(), _line_);
```

