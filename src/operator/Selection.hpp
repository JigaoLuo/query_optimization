#ifndef H_operator_Selection
#define H_operator_Selection
//---------------------------------------------------------------------------
#include "operator/Operator.hpp"
#include <memory>
//---------------------------------------------------------------------------
/// A selection
class Selection : public Operator
{
   private:
   /// The input
   std::unique_ptr<Operator> input;
   /// Registers of the condition
   const Register* condition;
   /// Second register for implicit equal tests
   const Register* equal;

   public:
   /// Constructor. Condition must be a bool value
   Selection(std::unique_ptr<Operator>&& input,const Register* condition);
   /// Constructor. Registers a and b are compared
   Selection(std::unique_ptr<Operator>&& input,const Register* a,const Register* b);
   /// Destructor
   ~Selection() override;

   /// Open the operator
   void open() override;
   /// Get the next tuple
   bool next() override;
   /// Close the operator
   void close() override;

    void print_type() override {input->print_type(); std::cout << "Selection\n";};
    void print_type_tail() override {std::cout << "Selection\n"; input->print_type_tail();};

   /// Get all produced values
   [[nodiscard]] std::vector<const Register*> getOutput() const override;
};
//---------------------------------------------------------------------------
#endif
