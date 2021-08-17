#include "SQLParser.hpp"
//---------------------------------------------------------------------------
SQLParserResult parse_and_analyse_sql_statement(Database& db, const std::string& sql) {
    SQLLexer lexer(sql);
    SQLParserResult ret;

    std::string tmpstr, tmpstr2, tmpstr3;

    // SELECT

    auto token = lexer.next();
    if (token.tt != SQLLexer::Select) {
        throw incorrect_sql_error("Expect \"SELECT\"");
    }
    token = lexer.next();

    if (token.tt == SQLLexer::Identifier) {
        ret.projections.emplace_back(token.str.value());
        token = lexer.next();

        while (token.tt == SQLLexer::Comma) {
            token = lexer.next();
            if (token.tt != SQLLexer::Identifier) {
                throw incorrect_sql_error("Expect identifier");
            }
            ret.projections.emplace_back(token.str.value());
            token = lexer.next();
        }
    } else {
        if (token.tt != SQLLexer::Asterisk) {
            throw incorrect_sql_error("Expect identifier or \"*\"");
        }
        token = lexer.next();
    }

    // FROM

    if (token.tt != SQLLexer::From) {
        throw incorrect_sql_error("Expect \"FROM\"");
    }
    token = lexer.next();

    if (token.tt != SQLLexer::Identifier) {
        throw incorrect_sql_error("Expect identifier");
    }
    tmpstr = token.str.value();
    token = lexer.next();

    if (token.tt != SQLLexer::Identifier) {
        throw incorrect_sql_error("Expect identifier");
    }
    ret.relations.emplace_back(std::pair{tmpstr, token.str.value()});
    token = lexer.next();

    while (token.tt == SQLLexer::Comma) {
        token = lexer.next();
        if (token.tt != SQLLexer::Identifier) {
            throw incorrect_sql_error("Expect identifier");
        }
        tmpstr = token.str.value();
        token = lexer.next();

        if (token.tt != SQLLexer::Identifier) {
            throw incorrect_sql_error("Expect identifier");
        }
        ret.relations.emplace_back(std::pair{tmpstr, token.str.value()});
        token = lexer.next();
    }

    // WHERE

    if (token.tt != SQLLexer::Where) {
        throw incorrect_sql_error("Expect \"WHERE\"");
    }
    token = lexer.next();

    if (token.tt != SQLLexer::Identifier) {
        throw incorrect_sql_error("Expect identifier");
    }
    tmpstr = token.str.value();
    token = lexer.next();

    if (token.tt != SQLLexer::Dot) {
        throw incorrect_sql_error("Expect \".\"");
    }
    token = lexer.next();

    if (token.tt != SQLLexer::Identifier) {
        throw incorrect_sql_error("Expect identifier");
    }
    tmpstr2 = token.str.value();
    token = lexer.next();

    if (token.tt != SQLLexer::Equal) {
        throw incorrect_sql_error("Expect \"=\"");
    }
    token = lexer.next();

    if (token.tt == SQLLexer::Identifier) {
        tmpstr3 = token.str.value();
        token = lexer.next();
        if (token.tt == SQLLexer::Dot) {
            token = lexer.next();

            if (token.tt != SQLLexer::Identifier) {
                throw incorrect_sql_error("Expect identifier");
            }
            ret.joinConditions.emplace_back(std::pair{tmpstr, tmpstr2}, std::pair{tmpstr3, token.str.value()});
            token = lexer.next();
        }
        else throw incorrect_sql_error("Expect \".\"");
    }
    else if(token.tt == SQLLexer::String) {
        tmpstr3 = token.str.value();
        token = lexer.next();
        ret.selections.emplace_back(std::pair{tmpstr, tmpstr2}, tmpstr3);
    }
    else throw incorrect_sql_error("Expect identifier or constant");

    while (token.tt == SQLLexer::And) {
        token = lexer.next();

        if (token.tt != SQLLexer::Identifier) {
            throw incorrect_sql_error("Expect identifier");
        }
        tmpstr = token.str.value();
        token = lexer.next();

        if (token.tt != SQLLexer::Dot) {
            throw incorrect_sql_error("Expect \".\"");
        }
        token = lexer.next();

        if (token.tt != SQLLexer::Identifier) {
            throw incorrect_sql_error("Expect identifier");
        }
        tmpstr2 = token.str.value();
        token = lexer.next();

        if (token.tt != SQLLexer::Equal) {
            throw incorrect_sql_error("Expect \"=\"");
        }
        token = lexer.next();

        if (token.tt == SQLLexer::Identifier) {
            tmpstr3 = token.str.value();
            token = lexer.next();
            if (token.tt == SQLLexer::Dot) {
                token = lexer.next();

                if (token.tt != SQLLexer::Identifier) {
                    throw incorrect_sql_error("Expect identifier");
                }
                ret.joinConditions.emplace_back(std::pair{tmpstr, tmpstr2}, std::pair{tmpstr3, token.str.value()});
                token = lexer.next();
            }
            else throw incorrect_sql_error("Expect \".\"");
        }
        else if(token.tt == SQLLexer::String) {
            tmpstr3 = token.str.value();
            token = lexer.next();
            ret.selections.emplace_back(std::pair{tmpstr, tmpstr2}, tmpstr3);
        }
        else throw incorrect_sql_error("Expect identifier");
    }

    // sanity check

    if (token.tt != SQLLexer::Eof) {
        throw incorrect_sql_error("Expect EOF");
    }

    for (const auto& relation: ret.relations) {  // relation existence check
        if(!db.hasTable(relation.first)) {
            throw incorrect_sql_error("Table \"" + relation.first + "\" not found");
        }
    }

    for (const auto& projection: ret.projections) {  // attributes existence check
        bool found = false;
        for (size_t i = 0, limit_i = ret.relations.size(); i < limit_i; ++i) {
            if(db.getTable(ret.relations[i].first).findAttribute(projection) >= 0) {
                if (found) {  // Ambiguous attribute
                    throw incorrect_sql_error("Attribute \"" + projection + "\" is ambiguous");
                }
                found = true;
                ret.link_projections.emplace_back(i);
            }
        }
        if (!found) {
            throw incorrect_sql_error("Attribute \"" + projection + "\" not found");
        }
    }

    for (const auto& joinCondition: ret.joinConditions) {  // attributes existence check
        for (size_t i = 0, limit_i = ret.relations.size(); i < limit_i; ++i) {
            if (ret.relations[i].second == joinCondition.first.first) {
                if(db.getTable(ret.relations[i].first).findAttribute(joinCondition.first.second) < 0) {
                    throw incorrect_sql_error("Attribute \"" + joinCondition.first.second + "\" not found");
                }
                ret.link_joinConditions.emplace_back(std::pair{i, 0});
                break;
            }
        }
        for (size_t i = 0, limit_i = ret.relations.size(); i < limit_i; ++i) {
            if (ret.relations[i].second == joinCondition.second.first) {
                if(db.getTable(ret.relations[i].first).findAttribute(joinCondition.second.second) < 0) {
                    throw incorrect_sql_error("Attribute \"" + joinCondition.second.second + "\" not found");
                }
                ret.link_joinConditions.back().second = i;
                break;
            }
        }
    }

    for (const auto& selection: ret.selections) {  // attributes existence check
        for (size_t i = 0, limit_i = ret.relations.size(); i < limit_i; ++i) {
            if (ret.relations[i].second == selection.first.first) {
                if (db.getTable(ret.relations[i].first).findAttribute(selection.first.second) < 0) {
                    throw incorrect_sql_error("Attribute \"" + selection.first.second + "\" not found");
                }
                ret.link_selections.emplace_back(i);
                break;
            }
        }
    }

    return ret;
}
//---------------------------------------------------------------------------
