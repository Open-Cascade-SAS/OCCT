// Copyright: 	Matra-Datavision 1992
// File:	ExprIntrp_yaccanal.hxx
// Created:	Fri Aug 21 12:21:43 1992
// Author:	Arnaud BOUZY
//		<adn>


#ifndef ExprIntrp_yaccanal_HeaderFile
#define ExprIntrp_yaccanal_HeaderFile

#include <ExprIntrp_Analysis.hxx>

#ifndef _ExprIntrp_API
# if !defined(WNT) || defined(__ExprIntrp_DLL) || defined(__Expr_DLL) || defined(__ExprIntrp_DLL) || defined(__math_DLL) || defined(__MathBase_DLL) || defined(__ElCLib_DLL) || defined(__ElSLib_DLL) || defined(__BSplCLib_DLL) || defined(__BSplSLib_DLL) || defined(__Smoothing_DLL) || defined(__SysBase_DLL) || defined(__Data_DLL) || defined(__PLib_DLL) || defined(__GeomAbs_DLL) || defined(__Poly_DLL) || defined(__CSLib_DLL) || defined(__Convert_DLL) || defined(__Bnd_DLL) || defined(__gp_DLL) || defined(__TColgp_DLL)
#  define __ExprIntrp_API Standard_EXPORT
#  define __ExprIntrp_APIEXTERN Standard_EXPORTEXTERN
# else
#  define __ExprIntrp_API Standard_IMPORT
#  define __ExprIntrp_APIEXTERN Standard_IMPORT
# endif  // __ExprIntrp_DLL
#endif  // __ExprIntrp_API

//extern __ExprIntrp_API Standard_EXPORT ExprIntrp_Analysis ExprIntrp_Recept;
#ifdef _ExprIntrp_Analysis_SourceFile
 __ExprIntrp_API ExprIntrp_Analysis ExprIntrp_Recept;
#else
 __ExprIntrp_APIEXTERN ExprIntrp_Analysis ExprIntrp_Recept;
#endif

#endif
