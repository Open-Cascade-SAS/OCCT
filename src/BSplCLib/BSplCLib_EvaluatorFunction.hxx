// File:	BSplCLib_EvaluatorFunction.hxx
// Created:	Mon Mar  3 10:11:41 1997
// Author:	Xavier BENVENISTE
//		<xab@zozox.paris1.matra-dtv.fr>

#ifndef _BSplCLib_EvaluatorFunction_HeaderFile
#define _BSplCLib_EvaluatorFunction_HeaderFile

#ifndef _Standard_Integer_HeaderFile
#include <Standard_Integer.hxx>
#endif
#ifndef _Standard_Real_HeaderFile
#include <Standard_Real.hxx>
#endif
#ifndef _Standard_PrimitiveTypes_HeaderFile
#include <Standard_PrimitiveTypes.hxx>
#endif

// History - C function pointer converted to a virtual class
// in order to get rid of usage of static functions and static data
class BSplCLib_EvaluatorFunction
{
public:

  //! Empty constructor
  BSplCLib_EvaluatorFunction () {}

  //! Destructor should be declared as virtual
  virtual ~BSplCLib_EvaluatorFunction () {}

  //! Function evaluation method to be defined by descendant
  virtual void Evaluate (const Standard_Integer theDerivativeRequest,
                         const Standard_Real*   theStartEnd,
                         const Standard_Real    theParameter,
                         Standard_Real&         theResult,
                         Standard_Integer&      theErrorCode) const = 0;

  //! Shortcut for function-call style usage
  void operator () (const Standard_Integer theDerivativeRequest,
                    const Standard_Real*   theStartEnd,
                    const Standard_Real    theParameter,
                    Standard_Real&         theResult,
                    Standard_Integer&      theErrorCode) const
  {
    Evaluate (theDerivativeRequest, theStartEnd, theParameter, theResult, theErrorCode);
  }

private:

  //! Copy constructor is declared private to forbid copying
  BSplCLib_EvaluatorFunction (const BSplCLib_EvaluatorFunction&) {}

  //! Assignment operator is declared private to forbid copying
  void operator = (const BSplCLib_EvaluatorFunction&) {}
};

#endif
