#ifndef H_operator_Chi
#define H_operator_Chi
//---------------------------------------------------------------------------
#include "operator/Operator.hpp"
#include "Register.hpp"
#include <memory>
//---------------------------------------------------------------------------
/// A chi operator
class Chi : public Operator
{
   public:
   /// An operation
   typedef void (*Operation)(const Register& a,const Register& b,Register& result);
   /// Add
   static void Add(const Register& a,const Register& b,Register& result);
   /// Divide
   static void Div(const Register& a,const Register& b,Register& result);
   /// Compare
   static void Equal(const Register& a,const Register& b,Register& result);
   /// Compare
   static void NotEqual(const Register& a, const Register& b, Register& result);
   /// Compare
   static void Less(const Register& a,const Register& b,Register& result);
   /// Compare
   static void LessOrEqual(const Register& a,const Register& b,Register& result);

   private:
   /// The input
   std::unique_ptr<Operator> input;
   /// The operation
   Operation op;
   /// The input of the operation
   const Register* left,*right;
   /// The output of the operation
   Register output;

   public:
   /// Constructor. Sample usage: new Chi(move(input),Chi::Add,reg1,reg2);
   Chi(std::unique_ptr<Operator>&& input,Operation op,const Register* left,const Register* right);
   /// Destructor
   ~Chi() override;

   /// Open the operation
   void open() override;
   /// Get the next tuple
   bool next() override;
   /// Close the operator
   void close() override;

    void print_type() override {input->print_type(); std::cout << "Chi\n";};
    void print_type_tail() override {std::cout << "Chi\n"; input->print_type_tail();};

    /// Get the produced value
   [[nodiscard]] const Register* getResult() const { return &output; }
   /// Get all produced values
   [[nodiscard]] std::vector<const Register*> getOutput() const override;
};
//---------------------------------------------------------------------------
#endif
