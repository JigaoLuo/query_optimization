//
// Created by shen on 13.11.20.
//

#include "SQLLexer.hpp"

SQLLexer::Token SQLLexer::next() {
    while (pos < len && (src[pos] == ' ' || src[pos] == '\t' || src[pos] == '\n')) {
        ++pos;
    }

    if (pos == len) {
       return {Eof, std::nullopt};
    }

    switch (src[pos]) {
        case '=':
            ++pos;
            return {Equal, std::nullopt};
        case '.':
            ++pos;
            return {Dot, std::nullopt};
        case ',':
            ++pos;
            return {Comma, std::nullopt};
        case '*':
            ++pos;
            return {Asterisk, std::nullopt};
        case 's':
            if (src.substr(pos, 6) == "select") {
                pos += 6;
                return {Select, std::nullopt};
            }
            else {
                break;
            }
        case 'f':
            if (src.substr(pos, 4) == "from") {
                pos += 4;
                return {From, std::nullopt};
            }
            else {
                break;
            }
        case 'w':
            if (src.substr(pos, 5) == "where") {
                pos += 5;
                return {Where, std::nullopt};
            }
            else {
                break;
            }
        case 'a':
            if (src.substr(pos, 3) == "and") {
                pos += 3;
                return {And, std::nullopt};
            }
            else {
                break;
            }
    }

    if (src[pos] == '\'') {
        uint64_t start = ++pos;
        uint64_t end = pos;
        while (end < src.size() && src[end] != '\'') {
            ++end;
        }
        pos = end + 1;
        return {String, src.substr(start, end - start)};
    }

    if (src[pos] >= '0' && src[pos] <= '9') {
        uint64_t start = pos;
        uint64_t end = pos + 1;
        while (end < src.size() && src[end] >= '0' && src[end] <= '9') {
            ++end;
        }
        pos = end;
        return {String, src.substr(start, end - start)};
    }

    uint64_t cnt = pos + 1;
    while (cnt < len && ((src[cnt] >= 'a' && src[cnt] <='z') ||
                         (src[cnt] >= 'A' && src[cnt] <='Z') ||
                         (src[cnt] >= '0' && src[cnt] <='9') ||
                         (src[cnt] == '_'))
          ) {
        ++cnt;
    }

    uint64_t tmppos = pos;
    pos = cnt;
    return {Identifier, src.substr(tmppos, cnt - tmppos)};
}
