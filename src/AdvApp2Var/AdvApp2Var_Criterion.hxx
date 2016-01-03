// Created on: 1997-01-15
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

#ifndef _AdvApp2Var_Criterion_HeaderFile
#define _AdvApp2Var_Criterion_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Real.hxx>
#include <AdvApp2Var_CriterionType.hxx>
#include <AdvApp2Var_CriterionRepartition.hxx>
#include <Standard_Boolean.hxx>
class AdvApp2Var_Patch;
class AdvApp2Var_Context;



//! this class contains a given criterion to be satisfied
class AdvApp2Var_Criterion 
{
public:

  DEFINE_STANDARD_ALLOC

  Standard_EXPORT virtual ~AdvApp2Var_Criterion();
  
  Standard_EXPORT virtual void Value (AdvApp2Var_Patch& P, const AdvApp2Var_Context& C) const = 0;
  
  Standard_EXPORT virtual Standard_Boolean IsSatisfied (const AdvApp2Var_Patch& P) const = 0;
  
  Standard_EXPORT Standard_Real MaxValue() const;
  
  Standard_EXPORT AdvApp2Var_CriterionType Type() const;
  
  Standard_EXPORT AdvApp2Var_CriterionRepartition Repartition() const;




protected:



  Standard_Real myMaxValue;
  AdvApp2Var_CriterionType myType;
  AdvApp2Var_CriterionRepartition myRepartition;


private:





};







#endif // _AdvApp2Var_Criterion_HeaderFile
