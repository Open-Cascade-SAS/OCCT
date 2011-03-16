// File:	AdvApp2Var_EvaluatorFunc2Var.hxx
// Created:	Thu Apr 18 10:31:26 1996
// Author:	Joelle CHAUVET
//		<jct@sgi38>


#ifndef _Standard_Integer_HeaderFile
#include <Standard_Integer.hxx>
#endif
#ifndef _Standard_Real_HeaderFile
#include <Standard_Real.hxx>
#endif
#ifndef _Standard_PrimitiveTypes_HeaderFile
#include <Standard_PrimitiveTypes.hxx>
#endif


#ifndef AdvApp2Var_EvaluatorFunc2Var_HeaderFile
#define AdvApp2Var_EvaluatorFunc2Var_HeaderFile

extern "C" {
typedef  void (* AdvApp2Var_EvaluatorFunc2Var) (Standard_Integer *,
					    // Dimension
					    Standard_Real    *,
					    // StartEnd[2] in U
					    Standard_Real    *,
					    // StartEnd[2] in V
					    Standard_Integer *,
					    // Choice of constante, 1 for U, 2 for V
					    Standard_Real    *,
					    // Value of constant parameter
					    Standard_Integer *,
					    // Number of parameters
					    Standard_Real    *,
					    // Values of parameters,
					    Standard_Integer *,
					    // Derivative Request in U
					    Standard_Integer *,
					    // Derivative Request in V
					    Standard_Real    *, 
					    // Result[Dimension]
					    Standard_Integer *) ;
                                            // Error Code


}


#endif
