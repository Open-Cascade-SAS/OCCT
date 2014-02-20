// Created on: 1992-08-21
// Created by: Arnaud BOUZY
// Copyright (c) 1992-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

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
