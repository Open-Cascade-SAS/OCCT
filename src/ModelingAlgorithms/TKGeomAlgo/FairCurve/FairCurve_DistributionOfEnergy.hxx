// Created on: 1996-01-22
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

#ifndef _FairCurve_DistributionOfEnergy_HeaderFile
#define _FairCurve_DistributionOfEnergy_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <gp_Pnt2d.hxx>
#include <math_FunctionSet.hxx>

//! Abstract class to use the Energy of an FairCurve
class FairCurve_DistributionOfEnergy : public math_FunctionSet
{
public:
  DEFINE_STANDARD_ALLOC

  //! returns the number of variables of the function.
  Standard_EXPORT virtual int NbVariables() const override;

  //! returns the number of equations of the function.
  Standard_EXPORT virtual int NbEquations() const override;

  Standard_EXPORT void SetDerivativeOrder(const int DerivativeOrder);

protected:
  Standard_EXPORT FairCurve_DistributionOfEnergy(
    const int                                         BSplOrder,
    const occ::handle<NCollection_HArray1<double>>&   FlatKnots,
    const occ::handle<NCollection_HArray1<gp_Pnt2d>>& Poles,
    const int                                         DerivativeOrder,
    const int                                         NbValAux = 0);

  int                                        MyBSplOrder;
  occ::handle<NCollection_HArray1<double>>   MyFlatKnots;
  occ::handle<NCollection_HArray1<gp_Pnt2d>> MyPoles;
  int                                        MyDerivativeOrder;
  int                                        MyNbVar;
  int                                        MyNbEqua;
  int                                        MyNbValAux;
};

#endif // _FairCurve_DistributionOfEnergy_HeaderFile
