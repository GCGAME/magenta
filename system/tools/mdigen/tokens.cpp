// Copyright 2017 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <string.h>
#include <stdio.h>

#include "tokens.h"

struct ReservedWord {
    TokenType   token;
    const char* word;
} reserved_words [] = {
    { TOKEN_INT8_TYPE,      "int8" },
    { TOKEN_UINT8_TYPE,     "uint8" },
    { TOKEN_INT16_TYPE,     "int16" },
    { TOKEN_UINT16_TYPE,    "uint16" },
    { TOKEN_INT32_TYPE,     "int32" },
    { TOKEN_UINT32_TYPE,    "uint32" },
    { TOKEN_INT64_TYPE,     "int64" },
    { TOKEN_UINT64_TYPE,    "uint64" },
    { TOKEN_BOOLEAN_TYPE,   "boolean" },
    { TOKEN_STRING_TYPE,    "string" },
    { TOKEN_ARRAY_TYPE,     "array" },
    { TOKEN_RANGE32_TYPE,   "range32" },
    { TOKEN_RANGE64_TYPE,   "range64" },
    { TOKEN_LIST_TYPE,      "list" },
    { TOKEN_TRUE,           "true" },
    { TOKEN_FALSE,          "false" },
    { TOKEN_INVALID,        nullptr },
};

TokenType find_reserved_word(std::string& string) {
    const char* str = string.c_str();
    ReservedWord* test = reserved_words;

    while (test->word) {
        if (!strcmp(str, test->word)) {
            return test->token;
        }
        test++;
    }
    return TOKEN_IDENTIFIER;
}

mdi_type_t Token::get_type_name() {
    switch (type) {
        case TOKEN_INT8_TYPE:
            return MDI_INT8;
        case TOKEN_UINT8_TYPE:
            return MDI_UINT8;
        case TOKEN_INT16_TYPE:
            return MDI_INT16;
        case TOKEN_UINT16_TYPE:
            return MDI_UINT16;
        case TOKEN_INT32_TYPE:
            return MDI_INT32;
        case TOKEN_UINT32_TYPE:
            return MDI_UINT32;
        case TOKEN_INT64_TYPE:
            return MDI_INT64;
        case TOKEN_UINT64_TYPE:
            return MDI_UINT64;
        case TOKEN_BOOLEAN_TYPE:
            return MDI_BOOLEAN;
        case TOKEN_STRING_TYPE:
            return MDI_STRING;
        case TOKEN_ARRAY_TYPE:
            return MDI_ARRAY;
        case TOKEN_RANGE32_TYPE:
            return MDI_RANGE32;
        case TOKEN_RANGE64_TYPE:
            return MDI_RANGE64;
        case TOKEN_LIST_TYPE:
            return MDI_LIST;
        default:
            return MDI_INVALID_TYPE;
    }
}

void Token::print() {
    switch (type) {
        case TOKEN_INVALID:
            printf("TOKEN_INVALID\n");
            break;
        case TOKEN_EOF:
            printf("TOKEN_EOF\n");
            break;
        case TOKEN_INT_LITERAL:
            printf("TOKEN_INT_LITERAL %" PRId64 "\n", int_value);
            break;
        case TOKEN_NEG_INT_LITERAL:
            printf("TOKEN_NEG_INT_LITERAL %" PRId64 "\n", int_value);
            break;
        case TOKEN_STRING_LITERAL:
            printf("TOKEN_STRING_LITERAL %s\n", string_value.c_str());
            break;
        case TOKEN_IDENTIFIER:
            printf("TOKEN_IDENTIFIER %s\n", string_value.c_str());
            break;
        case TOKEN_LIST_START:
            printf("TOKEN_LIST_START\n");
            break;
        case TOKEN_LIST_END:
            printf("TOKEN_LIST_END\n");
            break;
        case TOKEN_ARRAY_START:
            printf("TOKEN_ARRAY_START\n");
            break;
        case TOKEN_ARRAY_END:
            printf("TOKEN_ARRAY_END\n");
            break;
        case TOKEN_RANGE_START:
            printf("TOKEN_RANGE_START\n");
            break;
        case TOKEN_RANGE_END:
            printf("TOKEN_RANGE_END\n");
            break;
        case TOKEN_EQUALS:
            printf("TOKEN_EQUALS\n");
            break;
        case TOKEN_INT8_TYPE:
            printf("TOKEN_INT8_TYPE\n");
            break;
        case TOKEN_UINT8_TYPE:
            printf("TOKEN_UINT8_TYPE\n");
            break;
        case TOKEN_INT16_TYPE:
            printf("TOKEN_INT16_TYPE\n");
            break;
        case TOKEN_UINT16_TYPE:
            printf("TOKEN_UINT16_TYPE\n");
            break;
        case TOKEN_INT32_TYPE:
            printf("TOKEN_INT32_TYPE\n");
            break;
        case TOKEN_UINT32_TYPE:
            printf("TOKEN_UINT32_TYPE\n");
            break;
        case TOKEN_INT64_TYPE:
            printf("TOKEN_INT64_TYPE\n");
            break;
        case TOKEN_UINT64_TYPE:
            printf("TOKEN_UINT64_TYPE\n");
            break;
        case TOKEN_BOOLEAN_TYPE:
            printf("TOKEN_BOOLEAN_TYPE\n");
            break;
        case TOKEN_STRING_TYPE:
            printf("TOKEN_STRING_TYPE\n");
            break;
        case TOKEN_ARRAY_TYPE:
            printf("TOKEN_ARRAY_TYPE\n");
            break;
        case TOKEN_RANGE32_TYPE:
            printf("TOKEN_RANGE32_TYPE\n");
            break;
        case TOKEN_RANGE64_TYPE:
            printf("TOKEN_RANGE64_TYPE\n");
            break;
        case TOKEN_LIST_TYPE:
            printf("TOKEN_LIST_TYPE\n");
            break;
        case TOKEN_TRUE:
            printf("TOKEN_TRUE\n");
            break;
        case TOKEN_FALSE:
            printf("TOKEN_FALSE\n");
            break;
        default:
            printf("unknown token %d\n", type);
            break;
    }
}

Tokenizer::Tokenizer(std::ifstream& in_file)
    : in_file(in_file)
{
    memset(peek, 0, sizeof(peek));
}

char Tokenizer::next_char() {
    char ch;

    if (peek[0]) {
        ch = peek[0];
        peek[0] = peek[1];
        peek[1] = 0;
    } else if (in_file.eof()) {
        ch = EOF;
    } else {
        in_file.get(ch);
    }
    return ch;
}

char Tokenizer::peek_char() {
    if (!peek[0]) {
         in_file.get(peek[0]);
    }
    return peek[0];
}

void Tokenizer::eat_whitespace() {
    while (1) {
        while (isspace(peek_char())) {
            next_char();
        }
        // handle C style comments
        if (peek_char() == '/') {
            // consume the '/'
            next_char();
            char ch = peek_char();
            if (ch == '/') {
                // read until end of line
                while ((ch = next_char()) != EOF && ch != '\n' && ch != '\r') {}
                if (ch == EOF) {
                    break;
                }
            } else if (ch == '*') {
                next_char();    // consume '*'

                // look for "*/"
                while (1) {
                    while ((ch = next_char()) != EOF && ch != '*') {}
                    if (ch == EOF) {
                        return;
                    }
                    if (peek_char() == '/') {
                        // consume '/'
                        next_char();
                        break;
                    }
                }
            } else {
                // end of whitespace
                // put characters we read into peek
                peek[0] = '/';
                peek[1] = ch;
                return;
            }
        } else {
            break;
        }
    }
}

bool Tokenizer::parse_identifier(Token& token, char ch) {
    std::string string;
    string.append(1, ch);

    ch = peek_char();
    while (isalnum(ch) || ch == '-' || ch == '_') {
        next_char();
        string.append(1, ch);
        ch = peek_char();
    }

    token.type = find_reserved_word(string);
    token.string_value = string;
    return true;
}

bool Tokenizer::parse_integer(Token& token, char ch) {
    bool negative = false;
    bool hexadecimal = false;
    uint64_t value = 0;

    token.string_value.append(1, ch);

    if (ch == '-') {
        negative = true;
        ch = next_char();
        token.string_value.append(1, ch);
    } else if (ch == '0') {
        ch = next_char();
        token.string_value.append(1, ch);
        if (ch == 'x' || ch == 'X') {
            hexadecimal = true;
            ch = next_char();
            token.string_value.append(1, ch);
        }
    }

    // ch now contains highest order digit to parse
    int digit_count = 0;
    while (1) {
        int digit = -1;

        if (ch >= '0' && ch <= '9') {
            digit = ch - '0';
        } else if (hexadecimal) {
            if (ch >= 'A' && ch <= 'F') {
                digit = ch - 'A' + 10;
            } else if (ch >= 'a' && ch <= 'f') {
                digit = ch - 'a' + 10;
            }
        }

        if (digit < 0) {
            break;
        }

        if (hexadecimal) {
            value = 16 * value + digit;
        } else {
            value = 10 * value + digit;
        }

        if (++digit_count > 16) {
            fprintf(stderr, "integer value too large\n");
            return false;
        }

        ch = peek_char();
        if (!isdigit(ch) && !(hexadecimal &&
                              ((ch >= 'A' && ch <= 'F') ||
                               (ch >= 'a' && ch <= 'f')))) {
            break;
        }
        token.string_value.append(1, ch);
        next_char();
    }

    token.type = (negative ? TOKEN_NEG_INT_LITERAL : TOKEN_INT_LITERAL);
    if (negative && value > -INT64_MIN) {
        fprintf(stderr, "integer value too small\n");
        return false;
    }
    token.int_value = value;
    return true;
}

bool Tokenizer::parse_string(Token& token) {
    std::string string;
    char ch = next_char();

    while (ch != EOF) {
        if (ch == '\\') {
            ch = next_char();
            if (ch == EOF) {
                break;
            }
            switch (ch) {
                case 'a':
                    ch = '\a';
                    break;
                case 'b':
                    ch = '\b';
                    break;
                case 'f':
                    ch = '\f';
                    break;
                case 'n':
                    ch = '\n';
                    break;
                case 'r':
                    ch = '\r';
                    break;
                case 't':
                    ch = '\t';
                    break;
                case 'v':
                    ch = '\v';
                    break;
                case '\\':
                    ch = '\\';
                    break;
                case '\'':
                    ch = '\'';
                    break;
                case '\"':
                    ch = '\"';
                    break;
                case '?':
                    ch = '?';
                    break;
                default:
                    fprintf(stderr, "Unsupported escape sequence \\%c in string literal\n", ch);
                    return false;
            }
        } else if (ch == '\"') {
            token.type = TOKEN_STRING_LITERAL;
            token.string_value = string;
            return true;
        }
        string.append(1, ch);
        ch = next_char();
    }

    fprintf(stderr, "end of file during unterminated string\n");
    return false;
}

    // returns false if we cannot parse the next token
    // EOF is not considered an error
bool Tokenizer::next_token(Token& token) {
    eat_whitespace();
    char ch = next_char();
    bool result = true;

    if (isalpha(ch)) {
       result = parse_identifier(token, ch);
    } else if (isdigit(ch) || ch == '-') {
        result = parse_integer(token, ch);
    } else if (ch == '\"') {
        result = parse_string(token);
    } else {
        switch (ch) {
            case EOF:
                token.type = TOKEN_EOF;
                return true;
            case '{':
                token.type = TOKEN_LIST_START;
                break;
            case '}':
                token.type = TOKEN_LIST_END;
                break;
            case '[':
                token.type = TOKEN_ARRAY_START;
                break;
            case ']':
                token.type = TOKEN_ARRAY_END;
                break;
            case '(':
                token.type = TOKEN_RANGE_START;
                break;
            case ')':
                token.type = TOKEN_RANGE_END;
                break;
            case '=':
                token.type = TOKEN_EQUALS;
                break;
            default:
                fprintf(stderr, "invalid token \'%c\'\n", ch);
                result = false;
        }
        token.string_value.clear();
        token.string_value.append(1, ch);
    }

#if PRINT_TOKENS
    if (result) {
        token.print();
    }
#endif

    return result;
}
