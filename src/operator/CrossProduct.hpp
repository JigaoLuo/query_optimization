#ifndef H_operator_CrossProduct
#define H_operator_CrossProduct
//---------------------------------------------------------------------------
#include "operator/Operator.hpp"
#include <memory>
//---------------------------------------------------------------------------
/// A cross product
class CrossProduct : public Operator
{
   private:
   /// The input
   std::unique_ptr<Operator> left,right;
   /// Read the left side?
   bool readLeft;

   public:
   /// Constructor
   CrossProduct(std::unique_ptr<Operator>&& left,std::unique_ptr<Operator>&& right);
   /// Destructor
   ~CrossProduct() override;

   /// Open the operator
   void open() override;
   /// Get the next tuple
   bool next() override;
   /// Close the operator
   void close() override;

    void print_type() override {left->print_type(); right->print_type(); std::cout << "CrossProduct\n";};
    void print_type_tail() override {std::cout << "CrossProduct\n"; left->print_type_tail(); right->print_type_tail();};

   /// Get all produced values
   [[nodiscard]] std::vector<const Register*> getOutput() const override;
};
//---------------------------------------------------------------------------
#endif
