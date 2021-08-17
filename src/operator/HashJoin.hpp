#ifndef H_operator_HashJoin
#define H_operator_HashJoin
//---------------------------------------------------------------------------
#include "operator/Operator.hpp"
#include "Register.hpp"
#include <memory>
#include <unordered_map>
//---------------------------------------------------------------------------
/// A hash join
class HashJoin : public Operator
{
   private:
   /// The input
   std::unique_ptr<Operator> left,right;
   /// The registers
   const Register* leftValue,*rightValue;
   /// The copy mechanism
   std::vector<Register*> leftRegs;
   /// The hashtable
   std::unordered_multimap<Register,std::vector<Register>,Register::hash> table;
   /// Iterator
   std::unordered_multimap<Register,std::vector<Register>,Register::hash>::const_iterator iter,iterLimit;

   public:
   /// Constructor
   HashJoin(std::unique_ptr<Operator>&& left,std::unique_ptr<Operator>&& right,const Register* leftValue,const Register* rightValue);
   /// Destructor
   ~HashJoin() override;

   /// Open the operator
   void open() override;
   /// Get the next tuple
   bool next() override;
   /// Close the operator
   void close() override;

    void print_type() override {left->print_type(); right->print_type(); std::cout << "HashJoin\n";};
    void print_type_tail() override {std::cout << "HashJoin\n"; left->print_type_tail(); right->print_type_tail();};

    /// Get all produced values
   [[nodiscard]] std::vector<const Register*> getOutput() const override;
};
//---------------------------------------------------------------------------
#endif
