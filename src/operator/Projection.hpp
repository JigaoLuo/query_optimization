#ifndef H_operator_Projection
#define H_operator_Projection
//---------------------------------------------------------------------------
#include "operator/Operator.hpp"
#include <memory>
//---------------------------------------------------------------------------
/// A projection
class Projection : public Operator
{
   private:
   /// The input
   std::unique_ptr<Operator> input;
   /// The output
   std::vector<const Register*> output;

   public:
   /// Constructor
   Projection(std::unique_ptr<Operator>&& input,const std::vector<const Register*>& output);
   /// Destructor
   ~Projection() override;

   /// Open the operator
   void open() override;
   /// Get the next tuple
   bool next() override;
   /// Close the operator
   void close() override;

   void print_type() override {input->print_type(); std::cout << "Projection\n";};
   void print_type_tail() override {std::cout << "Projection\n"; input->print_type_tail();};

   /// Get all produced values
   [[nodiscard]] std::vector<const Register*> getOutput() const override;
};
//---------------------------------------------------------------------------
#endif
