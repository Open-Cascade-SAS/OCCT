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


#include <AdvApp2Var_Context.hxx>
#include <AdvApp2Var_Criterion.hxx>
#include <AdvApp2Var_Patch.hxx>

AdvApp2Var_Criterion::~AdvApp2Var_Criterion()
{}

//============================================================================
//function : MaxValue
//purpose  :
//============================================================================

Standard_Real AdvApp2Var_Criterion::MaxValue() const 
{
  return myMaxValue; 
}

//============================================================================
//function : Type
//purpose  :
//============================================================================

AdvApp2Var_CriterionType AdvApp2Var_Criterion::Type() const 
{
  return myType; 
}


//============================================================================
//function : Repartition
//purpose  :
//============================================================================

AdvApp2Var_CriterionRepartition AdvApp2Var_Criterion::Repartition() const 
{
  return myRepartition; 
}

