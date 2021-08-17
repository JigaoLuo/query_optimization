#include "CMakeHelper.hpp"
#include "Database.hpp"
#include "operator/Tablescan.hpp"
#include <iostream>
//---------------------------------------------------------------------------
using namespace std;
//---------------------------------------------------------------------------
int main()
{
   Database db;
   db.open(UNIDB_DEFAULT_PATH);
   Table& studenten=db.getTable("studenten");

   Tablescan scan(studenten);
   const Register* name=scan.getOutput("name");

   scan.open();
   while (scan.next())
      cout << name->getString() << endl;
   scan.close();
}
//---------------------------------------------------------------------------
