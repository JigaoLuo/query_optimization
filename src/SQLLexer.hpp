//
// Created by shen on 13.11.20.
//

#ifndef TINYDB_SQLLEXER_HPP
#define TINYDB_SQLLEXER_HPP

#include <string>
#include <optional>
#include <cstdint>

class SQLLexer {
public:
    enum TokenType {
        Select,
        From,
        Where,
        And,
        Dot,
        Comma,
        Asterisk,
        Equal,
        String,
        Identifier,
        Eof,
    };
    struct Token {
        std::optional<TokenType> tt;
        std::optional<std::string> str;
    };

    explicit SQLLexer(const std::string& sourceCode): src(sourceCode), len(sourceCode.length()) { }
    Token next();

private:
    std::string src;
    uint64_t pos = 0;
    uint64_t len;
};


#endif //TINYDB_SQLLEXER_HPP
