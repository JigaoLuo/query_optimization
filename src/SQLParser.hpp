#pragma once

#include <vector>
#include <set>
#include <string>
#include "Database.hpp"
#include <SQLLexer.hpp>

struct incorrect_sql_error : std::runtime_error {
   //semantic or syntactic errors
   using std::runtime_error::runtime_error;
};

struct SQLParserResult {
   using Relation = std::pair<std::string, std::string>; //relationName and binding
   using BindingAttribute = std::pair<std::string, std::string>; //bindingName and attribute
   using AttributeName = std::string;
   using Constant = std::string;
   std::vector<Relation> relations;
   std::vector<AttributeName> projections;
   std::vector<std::pair<BindingAttribute, Constant>> selections;
   std::vector<std::pair<BindingAttribute, BindingAttribute>> joinConditions;
   std::vector<size_t> link_projections;
   std::vector<size_t> link_selections;
   std::vector<std::pair<size_t, size_t>> link_joinConditions;
};

SQLParserResult parse_and_analyse_sql_statement(Database& db, const std::string& sql);
