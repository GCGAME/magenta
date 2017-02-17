// Copyright 2017 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#include <inttypes.h>

#include <fstream>
#include <string>

#include <magenta/mdi.h>

enum TokenType {
    TOKEN_INVALID = 0,
    TOKEN_EOF,              // returned at end of input
    TOKEN_INT_LITERAL,      // non-negative integer
    TOKEN_NEG_INT_LITERAL,  // negative integer
    TOKEN_STRING_LITERAL,
    TOKEN_IDENTIFIER,
    TOKEN_LIST_START,       // '{'
    TOKEN_LIST_END,         // '{'
    TOKEN_ARRAY_START,      // '['
    TOKEN_ARRAY_END,        // ']'
    TOKEN_CHILD_SEPARATOR,  // ','
    TOKEN_EQUALS,           // '='

    // type names
    TOKEN_INT8_TYPE,        // "int8"
    TOKEN_UINT8_TYPE,       // "uint8"
    TOKEN_INT16_TYPE,       // "int16"
    TOKEN_UINT16_TYPE,      // "uint16"
    TOKEN_INT32_TYPE,       // "int32"
    TOKEN_UINT32_TYPE,      // "uint32"
    TOKEN_INT64_TYPE,       // "int64"
    TOKEN_UINT64_TYPE,      // "uint64"
    TOKEN_BOOLEAN_TYPE,     // "boolean"
    TOKEN_STRING_TYPE,      // "string"
    TOKEN_ARRAY_TYPE,       // "array"
    TOKEN_LIST_TYPE,        // "list"

    // special values
    TOKEN_TRUE,             // "true"
    TOKEN_FALSE,            // "false"
};

TokenType find_reserved_word(std::string& string);

struct Token {
    TokenType   type;
    uint64_t    int_value;
    std::string string_value;   // raw string value

    // returns type for type name tokens
    mdi_type_t get_type_name();

    void print();
};

struct Tokenizer {
    std::ifstream& in_file;
    char peek[2];

    Tokenizer(std::ifstream& in_file);
    char next_char();
    char peek_char();
    void eat_whitespace();
    bool parse_identifier(Token& token, char ch);
    bool parse_integer(Token& token, char ch);
    bool parse_string(Token& token);

    // returns false if we cannot parse the next token
    // EOF is not considered an error
    bool next_token(Token& token);
};
