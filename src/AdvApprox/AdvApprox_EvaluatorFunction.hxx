// File:	AdvApprox_EvaluatorFunction.hxx
// Created:	Mon May 29 17:04:50 1995
// Author:	Xavier BENVENISTE
//		<xab@nonox>

#ifndef _Standard_Integer_HeaderFile
#include <Standard_Integer.hxx>
#endif
#ifndef _Standard_Real_HeaderFile
#include <Standard_Real.hxx>
#endif
#ifndef _Standard_PrimitiveTypes_HeaderFile
#include <Standard_PrimitiveTypes.hxx>
#endif
#ifndef _AdvApprox_EvaluatorFunction_HeaderFile
#define _AdvApprox_EvaluatorFunction_HeaderFile

// abv 01.04.2009: the C function pointer converted to a virtual class
// in order to get rid of usage of static functions and static data

//! Interface for a class implementing a function to be approximated
//! by AdvApprox_ApproxAFunction
class AdvApprox_EvaluatorFunction 
{
 public:
  
  //! Empty constructor
  AdvApprox_EvaluatorFunction () {}
  
  //! Destructor should be declared as virtual
  virtual ~AdvApprox_EvaluatorFunction () {}
  
  //! Function evaluation method to be defined by descendant
  virtual void Evaluate (Standard_Integer *Dimension,
		         Standard_Real     StartEnd[2],
                         Standard_Real    *Parameter,
                         Standard_Integer *DerivativeRequest,
                         Standard_Real    *Result, // [Dimension]
                         Standard_Integer *ErrorCode) = 0;

  //! Shortcut for function-call style usage
  void operator () (Standard_Integer *Dimension,
                    Standard_Real     StartEnd[2],
                    Standard_Real    *Parameter,
                    Standard_Integer *DerivativeRequest,
                    Standard_Real    *Result, // [Dimension]
                    Standard_Integer *ErrorCode)
  { Evaluate (Dimension, StartEnd, Parameter, DerivativeRequest, Result, ErrorCode); }
  
 private:

  //! Copy constructor is declared private to forbid copying
  AdvApprox_EvaluatorFunction (const AdvApprox_EvaluatorFunction&) {}

  //! Assignment operator is declared private to forbid copying
  void operator = (const AdvApprox_EvaluatorFunction&) {}
};

#endif
