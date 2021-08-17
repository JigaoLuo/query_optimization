#ifndef H_operator_IndexScan
#define H_operator_IndexScan
//---------------------------------------------------------------------------
#include "operator/Operator.hpp"
#include "Table.hpp"
//---------------------------------------------------------------------------
/// An indexscan operator
class Indexscan : public Operator
{
   private:
   /// The buffer size
   static const unsigned bufferSize = 4096;

   /// The table
   Table& table;
   /// The index
   std::map<Register,unsigned>& index;
   /// The iterator over the index
   std::map<Register,unsigned>::const_iterator iter,iterLimit;
   /// The bounds
   const Register* lowerBound,*upperBound;
   /// Buffer pointers
   unsigned bufferStart,bufferStop;
   /// Construction helper
   std::string buf;
   /// The output
   std::vector<Register> output;
   /// A small buffer
   char buffer[bufferSize];

   public:
   /// Constructor
   Indexscan(Table& scale,unsigned indexAttribute,const Register* lowerBounds,const Register* upperBounds);
   /// Destructor
   ~Indexscan() override;

   /// Open the operator
   void open() override;
   /// Get the next tuple
   bool next() override;
   /// Close the operator
   void close() override;

    void print_type() override {std::cout << "IndexScan\n";};
    void print_type_tail() override {std::cout << "IndexScan\n";};

    /// Get all produced values
   [[nodiscard]] std::vector<const Register*> getOutput() const override;
};
//---------------------------------------------------------------------------
#endif
