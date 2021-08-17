#ifndef H_operator_Tablescan
#define H_operator_Tablescan
//---------------------------------------------------------------------------
#include "Operator.hpp"
#include <string>
//---------------------------------------------------------------------------
class Table;
//---------------------------------------------------------------------------
/// A tablescan operator
class Tablescan : public Operator
{
   private:
   /// The buffer size
   static const unsigned bufferSize = 4096;

   /// The table
   Table& table;
   /// Buffer pointers
   unsigned bufferStart,bufferStop;
   /// The current position
   unsigned filePos;
   /// Construction helper
   std::string buf;
   /// The output
   std::vector<Register> output;
   /// A small buffer
   char buffer[bufferSize];

   public:
   /// Constructor
   explicit Tablescan(Table& table);
   /// Destructor
   ~Tablescan() override;

   /// Open the operator
   void open() override;
   /// Get the next tuple
   bool next() override;
   /// Close the operator
   void close() override;

    void print_type() override {std::cout << "TableScan\n";};
    void print_type_tail() override {std::cout << "TableScan\n";};

   /// Get the table
   Table& getTable() { return table; }
   /// Get all produced values
   [[nodiscard]] std::vector<const Register*> getOutput() const override;
   /// Get one produced value
   [[nodiscard]] const Register* getOutput(const std::string& name) const;
};
//---------------------------------------------------------------------------
#endif

