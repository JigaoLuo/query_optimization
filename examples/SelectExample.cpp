#include "CMakeHelper.hpp"
#include "Database.hpp"
#include "operator/Tablescan.hpp"
#include "operator/Selection.hpp"
#include <iostream>
//---------------------------------------------------------------------------
using namespace std;
//---------------------------------------------------------------------------
struct OperationResult {
   vector<unique_ptr<Register>> registerContainer;
   unique_ptr<Operator> result;
   const Register* name;
};
//---------------------------------------------------------------------------
OperationResult get_select(Database& db)
{
   OperationResult retval;
   Table& studenten=db.getTable("studenten");

   unique_ptr<Tablescan> scan(new Tablescan(studenten));
   retval.name=scan->getOutput("name");
   const Register* semester=scan->getOutput("semester");
   //Register two;
   auto& two = *retval.registerContainer.emplace_back(new Register());
   two.setInt(2);
   retval.result = make_unique<Selection>(move(scan),semester,&two);
   return retval;
}
//---------------------------------------------------------------------------
int main()
//select s.name from studenten s where s.semester = 2
{
   Database db;
   db.open(UNIDB_DEFAULT_PATH);
   
   auto result = get_select(db);
   auto& select = *result.result;
   
   select.open();
   while (select.next())
      cout << result.name->getString() << endl;
   select.close();
}
//---------------------------------------------------------------------------
