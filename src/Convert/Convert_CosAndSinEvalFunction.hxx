// File:	Convert_CosAnSinEvalFunction.hxx
// Created:	Wed Aug 23 14:31:24 1995
// Author:	Xavier BENVENISTE
//		<xab@nonox>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array1OfInteger.hxx>

#ifndef _Convert_CosAnSinEvalFunction_HeaderFile
#define _Convert_CosAnSinEvalFunction_HeaderFile

typedef void   Convert_CosAndSinEvalFunction(Standard_Real,
					     const Standard_Integer,
					     const TColgp_Array1OfPnt2d&,
					     const TColStd_Array1OfReal&, 
					     const TColStd_Array1OfInteger&,
					     Standard_Real Result[2]) ; 

#endif
