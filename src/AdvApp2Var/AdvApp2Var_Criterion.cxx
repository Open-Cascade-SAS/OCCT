// File:	AdvApp2Var_Criterion.cxx
// Created:	Wed Jan 15 10:04:41 1997
// Author:	Joelle CHAUVET
//		<jct@sgi38>


#include <AdvApp2Var_Criterion.ixx>


void AdvApp2Var_Criterion::Delete()
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

