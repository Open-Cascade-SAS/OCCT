// File:	Convert_PolynomialCosAndSin.hxx
// Created:	Tue Oct 10 15:28:11 1995
// Author:	Jacques GOUSSARD
//		<jag@bravox>


#include <TColStd_HArray1OfReal.hxx>
#include <TColStd_HArray1OfInteger.hxx>

#ifndef _Convert_PolynomialCosAnSin_HeaderFile
#define _Convert_PolynomialCosAnSin_HeaderFile

void BuildPolynomialCosAndSin
  (const Standard_Real,
   const Standard_Real,
   const Standard_Integer,
   Handle(TColStd_HArray1OfReal)&,
   Handle(TColStd_HArray1OfReal)&,
   Handle(TColStd_HArray1OfReal)&);

#endif


