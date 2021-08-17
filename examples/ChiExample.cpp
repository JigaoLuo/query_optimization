#include "CMakeHelper.hpp"
#include "Database.hpp"
#include "operator/Tablescan.hpp"
#include "operator/CrossProduct.hpp"
#include "operator/Selection.hpp"
#include "operator/Projection.hpp"
#include "operator/Printer.hpp"
#include "operator/Chi.hpp"
#include <iostream>
#include <vector>
//---------------------------------------------------------------------------
using namespace std;
//---------------------------------------------------------------------------
struct OperationResult {
   vector<unique_ptr<Register>> registerContainer;
   unique_ptr<Operator> result;
};
//---------------------------------------------------------------------------
OperationResult get_chi(Database& db) 
{
   OperationResult retval;
   Table& studenten=db.getTable("studenten");

   unique_ptr<Tablescan> scan(new Tablescan(studenten));
   const Register* semester=scan->getOutput("semester");
   const Register* name=scan->getOutput("name");

   // find all students where semester num. is not 2
   //Register two;
   auto& two = *retval.registerContainer.emplace_back(new Register());
   two.setInt(2);
   unique_ptr<Chi> chi(new Chi(move(scan),Chi::NotEqual,semester,&two));
   const Register* chiResult=chi->getResult();

   unique_ptr<Selection> select(new Selection(move(chi),chiResult));
   unique_ptr<Projection> project(new Projection(move(select),{name}));
   
   retval.result = move(project);
   return retval;
}
//---------------------------------------------------------------------------
int main()
//select s.name from studenten s where s.semester <> 2
{
   Database db;
   db.open(UNIDB_DEFAULT_PATH);
   
   auto result = get_chi(db);
   
   Printer out(move(result.result));

   out.open();
   while (out.next());
   out.close();

}
//---------------------------------------------------------------------------
