// Created on: 1995-10-10
// Created by: Jacques GOUSSARD
// Copyright (c) 1995-1999 Matra Datavision
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


