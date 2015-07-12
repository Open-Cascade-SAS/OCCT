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

#ifndef _GeomPlate_PlateG1Criterion_HeaderFile
#define _GeomPlate_PlateG1Criterion_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TColgp_SequenceOfXY.hxx>
#include <TColgp_SequenceOfXYZ.hxx>
#include <AdvApp2Var_Criterion.hxx>
#include <Standard_Real.hxx>
#include <AdvApp2Var_CriterionType.hxx>
#include <AdvApp2Var_CriterionRepartition.hxx>
#include <Standard_Boolean.hxx>
class AdvApp2Var_Patch;
class AdvApp2Var_Context;



//! this class contains a specific G1 criterion for GeomPlate_MakeApprox
class GeomPlate_PlateG1Criterion  : public AdvApp2Var_Criterion
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT GeomPlate_PlateG1Criterion(const TColgp_SequenceOfXY& Data, const TColgp_SequenceOfXYZ& G1Data, const Standard_Real Maximum, const AdvApp2Var_CriterionType Type = AdvApp2Var_Absolute, const AdvApp2Var_CriterionRepartition Repart = AdvApp2Var_Regular);
  
  Standard_EXPORT virtual void Value (AdvApp2Var_Patch& P, const AdvApp2Var_Context& C) const Standard_OVERRIDE;
  
  Standard_EXPORT virtual Standard_Boolean IsSatisfied (const AdvApp2Var_Patch& P) const Standard_OVERRIDE;




protected:





private:



  TColgp_SequenceOfXY myData;
  TColgp_SequenceOfXYZ myXYZ;


};







#endif // _GeomPlate_PlateG1Criterion_HeaderFile
