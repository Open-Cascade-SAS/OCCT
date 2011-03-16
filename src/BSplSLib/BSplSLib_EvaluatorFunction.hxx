// File:	BSplSLib_EvaluatorFunction.hxx
// Created:	Mon May 12 10:55:26 1997
// Author:	Xavier BENVENISTE
//		<xab@zozox.paris1.matra-dtv.fr>


#ifndef _BSplSLib_EvaluatorFunction_HeaderFile
#define _BSplSLib_EvaluatorFunction_HeaderFile


#ifndef _Standard_Integer_HeaderFile
#include <Standard_Integer.hxx>
#endif
#ifndef _Standard_Real_HeaderFile
#include <Standard_Real.hxx>
#endif
#ifndef _Standard_PrimitiveTypes_HeaderFile
#include <Standard_PrimitiveTypes.hxx>
#endif
typedef  void (* BSplSLib_EvaluatorFunction) (const Standard_Integer ,
					    // Derivative Request
					       const Standard_Real    ,
					    // UParameter
					      const Standard_Real     ,
					    // VParameter
					       Standard_Real    &, 
					    // Result
					    Standard_Integer & ) ;
                                            // Error Code







#endif
