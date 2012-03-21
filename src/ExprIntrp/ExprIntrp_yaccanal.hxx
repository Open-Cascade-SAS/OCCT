// Created on: 1992-08-21
// Created by: Arnaud BOUZY
// Copyright (c) 1992-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.



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
