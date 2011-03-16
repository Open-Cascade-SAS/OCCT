// File:	BSplCLib_EvaluatorFunction.hxx
// Created:	Mon Mar  3 10:11:41 1997
// Author:	Xavier BENVENISTE
//		<xab@zozox.paris1.matra-dtv.fr>


#ifndef _BSplCLib_EvaluatorFunction_HeaderFile
#define _BSplCLib_EvaluatorFunction_HeaderFile


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
typedef  void (* BSplCLib_EvaluatorFunction) (const Standard_Integer ,
					    // Derivative Request
					       const Standard_Real    *,
					    // StartEnd[2]
					       const Standard_Real    ,
					    // Parameter
					       Standard_Real    &, 
					    // Result
					    Standard_Integer & ) ;
                                            // Error Code




#endif
