// Created on: 1997-03-05
// Created by: Joelle CHAUVET
// Copyright (c) 1997-1999 Matra Datavision
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

#ifndef _GeomPlate_PlateG0Criterion_HeaderFile
#define _GeomPlate_PlateG0Criterion_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <gp_XY.hxx>
#include <NCollection_Sequence.hxx>
#include <gp_XYZ.hxx>
#include <AdvApp2Var_Criterion.hxx>
#include <AdvApp2Var_CriterionType.hxx>
#include <AdvApp2Var_CriterionRepartition.hxx>
class AdvApp2Var_Patch;
class AdvApp2Var_Context;

//! this class contains a specific G0 criterion for GeomPlate_MakeApprox
class GeomPlate_PlateG0Criterion : public AdvApp2Var_Criterion
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT GeomPlate_PlateG0Criterion(
    const NCollection_Sequence<gp_XY>&    Data,
    const NCollection_Sequence<gp_XYZ>&   G0Data,
    const double                          Maximum,
    const AdvApp2Var_CriterionType        Type   = AdvApp2Var_Absolute,
    const AdvApp2Var_CriterionRepartition Repart = AdvApp2Var_Regular);

  Standard_EXPORT virtual void Value(AdvApp2Var_Patch&         P,
                                     const AdvApp2Var_Context& C) const override;

  Standard_EXPORT virtual bool IsSatisfied(const AdvApp2Var_Patch& P) const override;

private:
  NCollection_Sequence<gp_XY>  myData;
  NCollection_Sequence<gp_XYZ> myXYZ;
};

#endif // _GeomPlate_PlateG0Criterion_HeaderFile
