#ifndef H_operator_Printer
#define H_operator_Printer
//---------------------------------------------------------------------------
#include "Operator.hpp"
#include <memory>
//---------------------------------------------------------------------------
/// Prints tuple attributes
class Printer : public Operator
{
   private:
   /// The input
   std::unique_ptr<Operator> input;
   /// Registers to print
   std::vector<const Register*> toPrint;

   public:
   /// Constructor
   explicit Printer(std::unique_ptr<Operator>&& input);
   /// Constructor
   Printer(std::unique_ptr<Operator>&& input,const std::vector<const Register*>& toPrint);

   /// Open the operator
   void open() override;
   /// Get the next ruple
   bool next() override;
   /// CLose the operator
   void close() override;

    void print_type() override {input->print_type(); std::cout << "Printer\n";};
    void print_type_tail() override {std::cout << "Printer\n"; input->print_type_tail();};

    /// Get all produced values
   [[nodiscard]] std::vector<const Register*> getOutput() const override;
};
//---------------------------------------------------------------------------
#endif
