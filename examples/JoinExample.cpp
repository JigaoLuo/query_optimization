#include "CMakeHelper.hpp"
#include "Database.hpp"
#include "operator/Tablescan.hpp"
#include "operator/CrossProduct.hpp"
#include "operator/Selection.hpp"
#include "operator/Projection.hpp"
#include "operator/Printer.hpp"
#include <iostream>
//---------------------------------------------------------------------------
using namespace std;
//---------------------------------------------------------------------------
struct OperationResult {
   unique_ptr<Operator> result;
};
//---------------------------------------------------------------------------
OperationResult get_join(Database& db) 
{
   Table& professoren=db.getTable("professoren");
   Table& vorlesungen=db.getTable("vorlesungen");

   unique_ptr<Tablescan> scanProfessoren(new Tablescan(professoren));
   unique_ptr<Tablescan> scanVorlesungen(new Tablescan(vorlesungen));

   const Register* name=scanProfessoren->getOutput("name");
   const Register* persnr=scanProfessoren->getOutput("persnr");
   const Register* titel=scanVorlesungen->getOutput("titel");
   const Register* gelesenvon=scanVorlesungen->getOutput("gelesenvon");

   unique_ptr<CrossProduct> cp(new CrossProduct(move(scanProfessoren),move(scanVorlesungen)));
   unique_ptr<Selection> select(new Selection(move(cp),persnr,gelesenvon));
   unique_ptr<Projection> project(new Projection(move(select),{name,titel}));
   
   return OperationResult{move(project)};
}
//---------------------------------------------------------------------------
int main()
//select p.name, v.titel from professoren p, vorlesungen v where p.persnr = v.gelesenvon
{
   Database db;
   db.open(UNIDB_DEFAULT_PATH);
   
   auto result = get_join(db);
   
   Printer out(move(result.result));

   out.open();
   while (out.next());
   out.close();
}
//---------------------------------------------------------------------------
