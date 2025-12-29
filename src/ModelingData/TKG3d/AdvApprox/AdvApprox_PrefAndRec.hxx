// Created on: 1996-11-14
// Created by: Philippe MANGIN
// Copyright (c) 1996-1999 Matra Datavision
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

#ifndef _AdvApprox_PrefAndRec_HeaderFile
#define _AdvApprox_PrefAndRec_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <NCollection_Array1.hxx>
#include <AdvApprox_Cutting.hxx>
#include <Standard_Boolean.hxx>

//! inherits class Cutting; contains a list of preferential points (pi)i
//! and a list of Recommended points used in cutting management.
//! if Cutting is necessary in [a,b], we cut at the di nearest from (a+b)/2
class AdvApprox_PrefAndRec : public AdvApprox_Cutting
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT AdvApprox_PrefAndRec(const NCollection_Array1<double>& RecomendedCut,
                                       const NCollection_Array1<double>& PrefferedCut,
                                       const double                      Weight = 5);

  //! cuting value is
  //! - the recommended point nerest of (a+b)/2
  //! if pi is in ]a,b[ or else
  //! -  the preferential point nearest of (a+b) / 2
  //! if pi is in ](r*a+b)/(r+1) , (a+r*b)/(r+1)[ where r = Weight
  //! -  or (a+b)/2 else.
  Standard_EXPORT bool Value(const double a, const double b, double& cuttingvalue) const override;

private:
  NCollection_Array1<double> myRecCutting;
  NCollection_Array1<double> myPrefCutting;
  double                     myWeight;
};

#endif // _AdvApprox_PrefAndRec_HeaderFile
