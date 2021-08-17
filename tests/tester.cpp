#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch2/catch.hpp"
#include <string>
#include <set>
#include <vector>
#include <sstream>
#include "SQLParser.hpp"
#include "Database.hpp"
#include "CMakeHelper.hpp"
#include "SQLToOperator.hpp"
#include "Register.hpp"
#include "QueryGraph.hpp"
#include "operator/Printer.hpp"

using namespace std;

template <typename T>
vector<T> sorted(vector<T> vals) {
    sort(vals.begin(), vals.end());
    return vals;
}

template <typename T>
vector<T> sorted2(vector<T> vals) {
    for (auto& v : vals) {
        if (v.first > v.second) {
            swap(v.first, v.second);
        }
    }
    sort(vals.begin(), vals.end());
    return vals;
}

namespace Catch {
template<typename F, typename S>
struct StringMaker<std::pair<F,S>> {
	static std::string convert(const std::pair<F,S>& value)
	{
		return "{"s +
            StringMaker<decltype(value.first)>().convert(value.first) +
            ", " +
            StringMaker<decltype(value.second)>().convert(value.second) +
            "}";
	}
};
template<>
struct StringMaker<Register> {
	static std::string convert(const Register& r)
	{
                stringstream result;
                result << "Register(";
        switch (r.getState()) {
            case Register::State::Bool:
                result << (r.getBool() ? "TRUE" : "FALSE");
                break;
            case Register::State::Int:
                result << r.getInt();
                break;
            case Register::State::Double:
                result << r.getDouble();
                break;
            case Register::State::String:
                result << "'" + r.getString() + "'";
                break;
            case Register::State::Unbound:
                result << "UNBOUND";
                break;
        }
        result << ")";
        return result.str();
	}
};
}

string sql01 = "select * from studenten s1, studenten s2, hoeren h1, hoeren h2 where s1.matrnr = h1.matrnr and s2.matrnr = h2.matrnr and h1.vorlnr = h2.vorlnr and s2.matrnr=s1.matrnr and s1.name = 'Schopenhauer'";
string sql02 = "select titel from vorlesungen v, studenten s, hoeren h where s.matrnr=h.matrnr and v.vorlnr = h.vorlnr and s.semester = 2";
string sql03 = "select * from studenten s1, studenten s2, hoeren h1, hoeren h2 where s1.matrnr = h1.matrnr and s2.matrnr = h2.matrnr and h1.vorlnr = h2.vorlnr and s1.name = 'Schopenhauer'";
string sql04 = "select vorlnr, titel from vorlesungen v where v.vorlnr = 5041";
string sql04a = "select vorlnr, titel from vorlesungen v where v.titel = 'Logik'";
string sql04b = "select vorlnr, titel from vorlesungen v where v.sws = 4";
string sql05 = "select vorlnr, titel from vorlesungen v where v.vorlnr = 5041 and v.vorlnr = 5031";
string sql06 = "select vorlnr, titel, persnr from vorlesungen v where v.vorlnr = 5041";
string sql07 = "select vorlnr, titel from vorlesungen v where v.vorlnr = 5041 and v.persnr = 5";
string sql09 = "select titel from vorlesungen titel where titel.titel = 'Ethik'";
string sql10 = "select matrnr from studenten s1, studenten s2, hoeren h1, hoeren h2 where s1.matrnr = h1.matrnr and s2.matrnr = h2.matrnr and h1.vorlnr = h2.vorlnr and s2.matrnr = s1.matrnr and s1.name = 'Schopenhauer'";
string sql11 = "select vorlnr titel from vorlesungen v where v.vorlnr = 5041";
string sql12 = "select, vorlnr from vorlesungen v where v.vorlnr = 5041";
string sql13 = "select vorlnr, titel from vorlesungen v where v.vorlnr 5041";
string sql14 = "select vorlnr,,,, titel from vorlesungen v where v.vorlnr 5041";
string sql15 = "select vorlnr, titel f vorlesungen v where v.vorlnr = 5041";
string sql16 = "select from vorlesungen v where v.vorlnr = 5041";
string sql17 = "select name, titel from vorlesungen v, studenten s, hoeren h where s.matrnr=h.matrnr and v.vorlnr = h.vorlnr";
string sql18 = "select name, titel from vorlesungen v, professoren p where p.persnr=v.gelesenvon and p.rang='C4' and v.sws=3";
string sql19 = "select raum, note from pruefen p, assistenten a, professoren prof where a.boss=p.persnr and prof.persnr=a.boss and p.persnr=prof.persnr and p.note=2";

string tpch1="select * from lineitem l, orders o, customer c where l.l_orderkey=o.o_orderkey and o.o_custkey=c.c_custkey and c.c_name='Customer#000014993'";

TEST_CASE( "Relations and bindings are correctly read", "[sql][relations]" ) {
    Database db;
    db.open(UNIDB_DEFAULT_PATH);
    using RT = decltype(SQLParserResult::relations);
    CHECK( sorted(parse_and_analyse_sql_statement(db, sql01).relations)
        == sorted(RT{{"studenten","s1"},{"studenten","s2"},{"hoeren","h1"},{"hoeren","h2"}}) );
    CHECK( sorted(parse_and_analyse_sql_statement(db, sql02).relations)
        == sorted(RT{{"vorlesungen","v"},{"studenten","s"},{"hoeren","h"}}) );
    CHECK( sorted(parse_and_analyse_sql_statement(db, sql03).relations)
        == sorted(RT{{"studenten","s1"},{"studenten","s2"},{"hoeren","h1"},{"hoeren","h2"}}) );
    CHECK( sorted(parse_and_analyse_sql_statement(db, sql04).relations)
        == sorted(RT{{"vorlesungen","v"}}) );
    CHECK( sorted(parse_and_analyse_sql_statement(db, sql05).relations)
        == sorted(RT{{"vorlesungen","v"}}) );
    CHECK( sorted(parse_and_analyse_sql_statement(db, sql09).relations)
        == sorted(RT{{"vorlesungen","titel"}}) );
}

TEST_CASE( "Projection attributes are correctly read", "[sql][projections]" ) {
    Database db;
    db.open(UNIDB_DEFAULT_PATH);
    using RT = decltype(SQLParserResult::projections);
    CHECK( sorted(parse_and_analyse_sql_statement(db, sql01).projections)
        == sorted(RT{}) );
    CHECK( sorted(parse_and_analyse_sql_statement(db, sql02).projections)
        == sorted(RT{"titel"}) );
    CHECK( sorted(parse_and_analyse_sql_statement(db, sql03).projections)
        == sorted(RT{}) );
    CHECK( sorted(parse_and_analyse_sql_statement(db, sql04).projections)
        == sorted(RT{"vorlnr","titel"}) );
    CHECK( sorted(parse_and_analyse_sql_statement(db, sql05).projections)
        == sorted(RT{"vorlnr","titel"}) );
    CHECK( sorted(parse_and_analyse_sql_statement(db, sql09).projections)
        == sorted(RT{"titel"}) );
}

TEST_CASE( "Selections are correctly read", "[sql][selections]" ) {
    Database db;
    db.open(UNIDB_DEFAULT_PATH);
    using RT = decltype(SQLParserResult::selections);
    CHECK( sorted(parse_and_analyse_sql_statement(db, sql01).selections)
        == sorted(RT{{{"s1","name"},"Schopenhauer"}}) );
    CHECK( sorted(parse_and_analyse_sql_statement(db, sql02).selections)
        == sorted(RT{{{"s","semester"},"2"}}) );
    CHECK( sorted(parse_and_analyse_sql_statement(db, sql03).selections)
        == sorted(RT{ {{"s1","name"},"Schopenhauer"} }) );
    CHECK( sorted(parse_and_analyse_sql_statement(db, sql04).selections)
        == sorted(RT{{{"v","vorlnr"},"5041"}}) );
    CHECK( sorted(parse_and_analyse_sql_statement(db, sql05).selections)
        == sorted(RT{{{"v","vorlnr"},"5041"},{{"v","vorlnr"},"5031"}}) );
    CHECK( sorted(parse_and_analyse_sql_statement(db, sql09).selections)
        == sorted(RT{{{"titel","titel"},"Ethik"}}) );
}

TEST_CASE( "Join conditions are correctly read", "[sql][joinConditions]" ) {
    Database db;
    db.open(UNIDB_DEFAULT_PATH);
    using RT = decltype(SQLParserResult::joinConditions);
    CHECK( sorted2(parse_and_analyse_sql_statement(db, sql01).joinConditions)
        == sorted2(RT{ {{"s1","matrnr"},{"h1","matrnr"}}, {{"s2","matrnr"},{"h2","matrnr"}}, {{"h1","vorlnr"},{"h2","vorlnr"}}, {{"s2","matrnr"},{"s1","matrnr"}} }) );
    CHECK( sorted2(parse_and_analyse_sql_statement(db, sql02).joinConditions)
        == sorted2(RT{ {{"s","matrnr"},{"h","matrnr"}}, {{"v","vorlnr"},{"h","vorlnr"}} }) );
    CHECK( sorted2(parse_and_analyse_sql_statement(db, sql03).joinConditions)
        == sorted2(RT{ {{"s1","matrnr"},{"h1","matrnr"}}, {{"s2","matrnr"},{"h2","matrnr"}}, {{"h1","vorlnr"},{"h2","vorlnr"}} }) );
    CHECK( sorted2(parse_and_analyse_sql_statement(db, sql04).joinConditions)
        == sorted2(RT{}) );
    CHECK( sorted2(parse_and_analyse_sql_statement(db, sql05).joinConditions)
        == sorted2(RT{}) );
    CHECK( sorted2(parse_and_analyse_sql_statement(db, sql09).joinConditions)
        == sorted2(RT{}) );
}

TEST_CASE( "Semantic errors are checked for", "[sql][syntax]" ) {
    Database db;
    db.open(UNIDB_DEFAULT_PATH);
    CHECK_THROWS(parse_and_analyse_sql_statement(db, sql06));
    CHECK_THROWS(parse_and_analyse_sql_statement(db, sql07));
    CHECK_THROWS(parse_and_analyse_sql_statement(db, sql10));

    CHECK_THROWS(parse_and_analyse_sql_statement(db, sql11));
    CHECK_THROWS(parse_and_analyse_sql_statement(db, sql12));
    CHECK_THROWS(parse_and_analyse_sql_statement(db, sql13));
    CHECK_THROWS(parse_and_analyse_sql_statement(db, sql14));
    CHECK_THROWS(parse_and_analyse_sql_statement(db, sql15));
    CHECK_THROWS(parse_and_analyse_sql_statement(db, sql16));
}

Register make_reg(const string& v) {
    Register r;
    r.setString(v);
    return r;
}
Register make_reg(int v) {
    Register r;
    r.setInt(v);
    return r;
}
Register make_reg(double v) {
    Register r;
    r.setDouble(v);
    return r;
}
Register make_reg(bool v) {
    Register r;
    r.setBool(v);
    return r;
}


vector<vector<Register>> executeImpl(const std::unique_ptr<Operator>& root) {
    vector<vector<Register>> mv;
    root->open();
    while (root->next()) {
        vector<Register> tup;
        for (const Register* r : root->getOutput()) {
            tup.emplace_back(*r);
        }
        mv.emplace_back(move(tup));
    }
    root->close();
    sort(mv.begin(), mv.end());
    return mv;
}

size_t print(unique_ptr<Operator>&& root) {
   size_t result=0;
   auto printer=make_unique<Printer>(move(root));
   printer->open();
   while (printer->next()) ++result;
   printer->close();
   return result;
}

TEST_CASE( "SQL Parse and Execute", "[sql][execution]" ) {
    Database db;
    db.open(UNIDB_DEFAULT_PATH);
    auto container=sql_to_plan(db,sql02);
    REQUIRE(container.root);
    CHECK( executeImpl(container.root) == vector<vector<Register>>{
          {make_reg("Ethik"s)},
          {make_reg("Glaube und Wissen"s)},
          {make_reg("Grundzüge"s)},
          {make_reg("Grundzüge"s)},
          {make_reg("Mäeutik"s)}});

    container = sql_to_plan(db, sql04);
    REQUIRE(container.root);
    CHECK( executeImpl(container.root) == vector<vector<Register>>{
          {make_reg(5041),make_reg("Ethik"s)}});

    container = sql_to_plan(db, sql04a);
    REQUIRE(container.root);
    CHECK( executeImpl(container.root) == vector<vector<Register>>{
          {make_reg(4052),make_reg("Logik"s)}});

    container = sql_to_plan(db, sql05);
    REQUIRE(container.root);
    CHECK( executeImpl(container.root) == vector<vector<Register>>{});

    container = sql_to_plan(db, sql17);
    REQUIRE(container.root);
    CHECK( executeImpl(container.root) == vector<vector<Register>>{
          {make_reg("Carnap"s),make_reg("Bioethik"s)},
          {make_reg("Carnap"s),make_reg("Der Wiener Kreis"s)},
          {make_reg("Carnap"s),make_reg("Ethik"s)},
          {make_reg("Carnap"s),make_reg("Wissenschaftstheorie"s)},
          {make_reg("Feuerbach"s),make_reg("Glaube und Wissen"s)},
          {make_reg("Feuerbach"s),make_reg("Grundzüge"s)},
          {make_reg("Fichte"s),make_reg("Grundzüge"s)},
          {make_reg("Jonas"s),make_reg("Glaube und Wissen"s)},
          {make_reg("Schopenhauer"s),make_reg("Grundzüge"s)},
          {make_reg("Schopenhauer"s),make_reg("Logik"s)},
          {make_reg("Theophrastos"s),make_reg("Ethik"s)},
          {make_reg("Theophrastos"s),make_reg("Grundzüge"s)},
          {make_reg("Theophrastos"s),make_reg("Mäeutik"s)}});
}

TEST_CASE( "SQL to graph", "[sql][querygraph]" ) {
    Database db;
    db.open(UNIDB_DEFAULT_PATH);
    db.runStats();

    //select titel from vorlesungen v, studenten s, hoeren h where s.matrnr=h.matrnr and v.vorlnr = h.vorlnr and s.semester = 2
    auto graph=sql_to_graph(db,sql02);
    CHECK(graph.getCardinality("v")==Approx(10.0));
    CHECK(graph.getCardinality("s")==Approx(1.142857));
    CHECK(graph.getCardinality("h")==Approx(13.0));
    CHECK(graph.getCardinality("x")==Approx(-1.0));

    CHECK(graph.getSelectivity("v","h")==Approx(0.1));
    CHECK(graph.getSelectivity("h","v")==Approx(0.1));
    CHECK(graph.getSelectivity("v","s")==Approx(1.0));
    CHECK(graph.getSelectivity("s","v")==Approx(1.0));
    CHECK(graph.getSelectivity("s","h")==Approx(0.125));
    CHECK(graph.getSelectivity("h","s")==Approx(0.125));
    CHECK(graph.getSelectivity("x","y")==Approx(1.0));

    //select vorlnr, titel from vorlesungen v where v.vorlnr = 5041
    graph=sql_to_graph(db,sql04);
    CHECK(graph.getCardinality("v")==Approx(1.0));

    //select vorlnr, titel from vorlesungen v where v.titel = 'Logik'
    graph=sql_to_graph(db,sql04b);
    CHECK(graph.getCardinality("v")==Approx(3.33333));

    //select vorlnr, titel from vorlesungen v where v.vorlnr = 5041 and v.vorlnr = 5031
    graph=sql_to_graph(db,sql05);
    CHECK(graph.getCardinality("v")==Approx(0.1));

    //select name, titel from vorlesungen v, studenten s, hoeren h where s.matrnr=h.matrnr and v.vorlnr = h.vorlnr
    graph=sql_to_graph(db,sql17);
    CHECK(graph.getCardinality("v")==Approx(10.0));
    CHECK(graph.getCardinality("s")==Approx(8.0));
    CHECK(graph.getCardinality("h")==Approx(13.0));

    CHECK(graph.getSelectivity("v","h")==Approx(0.1));
    CHECK(graph.getSelectivity("h","v")==Approx(0.1));
    CHECK(graph.getSelectivity("v","s")==Approx(1.0));
    CHECK(graph.getSelectivity("s","v")==Approx(1.0));
    CHECK(graph.getSelectivity("s","h")==Approx(0.125));
    CHECK(graph.getSelectivity("h","s")==Approx(0.125));

    //select name, titel from vorlesungen v, professoren p where p.persnr=v.gelesenvon and p.rang='C4' and v.sws=3
    graph=sql_to_graph(db,sql18);
    CHECK(graph.getCardinality("v")==Approx(3.33333));
    CHECK(graph.getCardinality("p")==Approx(3.5));

    CHECK(graph.getSelectivity("v","p")==Approx(0.142857));
    CHECK(graph.getSelectivity("p","v")==Approx(0.142857));

    //select raum, note from pruefen p, assistenten a, professoren prof where a.boss=p.persnr and prof.persnr=a.boss and p.persnr=prof.persnr and note=2
    graph=sql_to_graph(db,sql19);
    CHECK(graph.getCardinality("p")==Approx(1.5));
    CHECK(graph.getCardinality("a")==Approx(6.0));
    CHECK(graph.getCardinality("prof")==Approx(7.0));

    CHECK(graph.getSelectivity("p","a")==Approx(0.25));
    CHECK(graph.getSelectivity("a","p")==Approx(0.25));
    CHECK(graph.getSelectivity("p","prof")==Approx(0.142857));
    CHECK(graph.getSelectivity("prof","p")==Approx(0.142857));
    CHECK(graph.getSelectivity("a","prof")==Approx(0.142857));
    CHECK(graph.getSelectivity("prof","a")==Approx(0.142857));
}

TEST_CASE( "GOO", "[sql][GOO]" ) {
    Database db;
    db.open(UNIDB_DEFAULT_PATH);
    db.runStats();

    auto graph=sql_to_graph(db,sql02);
    std::cout << "sql02" << std::endl;
    auto optimized=graph.runGOO();

    REQUIRE(optimized.root);
    CHECK(optimized.cost==Approx(3.7142857));
    CHECK( executeImpl(optimized.root) == vector<vector<Register>>{
          {make_reg("Ethik"s)},
          {make_reg("Glaube und Wissen"s)},
          {make_reg("Grundzüge"s)},
          {make_reg("Grundzüge"s)},
          {make_reg("Mäeutik"s)}});

    graph=sql_to_graph(db,sql04);
    std::cout << "sql04" << std::endl;
    optimized=graph.runGOO();

    REQUIRE(optimized.root);
    CHECK(optimized.cost==Approx(0.0));
    CHECK( executeImpl(optimized.root) == vector<vector<Register>>{
          {make_reg(5041),make_reg("Ethik"s)}});

    graph=sql_to_graph(db,sql17);
    std::cout << "sql17" << std::endl;
    optimized=graph.runGOO();

    REQUIRE(optimized.root);
    CHECK(optimized.cost==Approx(26.0));
    CHECK( executeImpl(optimized.root) == vector<vector<Register>>{
          {make_reg("Carnap"s),make_reg("Bioethik"s)},
          {make_reg("Carnap"s),make_reg("Der Wiener Kreis"s)},
          {make_reg("Carnap"s),make_reg("Ethik"s)},
          {make_reg("Carnap"s),make_reg("Wissenschaftstheorie"s)},
          {make_reg("Feuerbach"s),make_reg("Glaube und Wissen"s)},
          {make_reg("Feuerbach"s),make_reg("Grundzüge"s)},
          {make_reg("Fichte"s),make_reg("Grundzüge"s)},
          {make_reg("Jonas"s),make_reg("Glaube und Wissen"s)},
          {make_reg("Schopenhauer"s),make_reg("Grundzüge"s)},
          {make_reg("Schopenhauer"s),make_reg("Logik"s)},
          {make_reg("Theophrastos"s),make_reg("Ethik"s)},
          {make_reg("Theophrastos"s),make_reg("Grundzüge"s)},
          {make_reg("Theophrastos"s),make_reg("Mäeutik"s)}});

    graph=sql_to_graph(db,sql19);
    std::cout << "sql19" << std::endl;
    optimized=graph.runGOO();

    REQUIRE(optimized.root);
    CHECK(optimized.cost==Approx(1.821428));
    CHECK( executeImpl(optimized.root) == vector<vector<Register>>{
          {make_reg(226),make_reg(2)},
          {make_reg(226),make_reg(2)}});
}

TEST_CASE( "DP", "[sql][DP]" ) {
   Database db;
   db.open(UNIDB_DEFAULT_PATH);
   db.runStats();

   {
      auto graph=sql_to_graph(db,sql02);
      std::cout << "sql02" << std::endl;
      auto optimized=graph.runDP();

      REQUIRE(optimized.root);
      CHECK(optimized.cost==Approx(3.7142857));
      CHECK( executeImpl(optimized.root) == vector<vector<Register>>{
            {make_reg("Ethik"s)},
            {make_reg("Glaube und Wissen"s)},
            {make_reg("Grundzüge"s)},
            {make_reg("Grundzüge"s)},
            {make_reg("Mäeutik"s)}});
   }

   {
      auto graph=sql_to_graph(db,sql04);
      std::cout << "sql04" << std::endl;
      auto optimized=graph.runDP();

      REQUIRE(optimized.root);
      CHECK(optimized.cost==Approx(0.0));
      CHECK( executeImpl(optimized.root) == vector<vector<Register>>{
            {make_reg(5041),make_reg("Ethik"s)}});
   }

   {
      auto graph=sql_to_graph(db,sql17);
      std::cout << "sql17" << std::endl;
      auto optimized=graph.runDP();

      REQUIRE(optimized.root);
      CHECK(optimized.cost==Approx(26.0));
      CHECK( executeImpl(optimized.root) == vector<vector<Register>>{
            {make_reg("Carnap"s),make_reg("Bioethik"s)},
            {make_reg("Carnap"s),make_reg("Der Wiener Kreis"s)},
            {make_reg("Carnap"s),make_reg("Ethik"s)},
            {make_reg("Carnap"s),make_reg("Wissenschaftstheorie"s)},
            {make_reg("Feuerbach"s),make_reg("Glaube und Wissen"s)},
            {make_reg("Feuerbach"s),make_reg("Grundzüge"s)},
            {make_reg("Fichte"s),make_reg("Grundzüge"s)},
            {make_reg("Jonas"s),make_reg("Glaube und Wissen"s)},
            {make_reg("Schopenhauer"s),make_reg("Grundzüge"s)},
            {make_reg("Schopenhauer"s),make_reg("Logik"s)},
            {make_reg("Theophrastos"s),make_reg("Ethik"s)},
            {make_reg("Theophrastos"s),make_reg("Grundzüge"s)},
            {make_reg("Theophrastos"s),make_reg("Mäeutik"s)}});
   }

   {
      auto graph=sql_to_graph(db,sql19);
      std::cout << "sql19" << std::endl;
      auto optimized=graph.runDP();

      REQUIRE(optimized.root);
      CHECK(optimized.cost==Approx(1.821428));
      CHECK( executeImpl(optimized.root) == vector<vector<Register>>{
            {make_reg(226),make_reg(2)},
            {make_reg(226),make_reg(2)}});
   }
}

TEST_CASE( "TPCH-1", "[sql][TPCH]" ) {
   Database db;
   db.open(TPCH_DEFAULT_PATH);
   db.runStats();

   auto graph=sql_to_graph(db,tpch1);
   std::cout << "tpch1" << std::endl;
   auto optimized=graph.runDP();

   REQUIRE(optimized.root);
   CHECK(optimized.cost==Approx(50.0381));
   CHECK(print(move(optimized.root))==34);
}
