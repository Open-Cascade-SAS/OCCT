// File:	ShapeUpgrade_Tool.cxx
// Created:	Tue Aug 31 15:16:08 1999
// Author:	Pavel DURANDIN
//		<pdn@friendox.nnov.matra-dtv.fr>


#include <ShapeUpgrade_Tool.ixx>
#include <Precision.hxx>

//=======================================================================
//function : ShapeUpgrade_Tool
//purpose  : 
//=======================================================================

ShapeUpgrade_Tool::ShapeUpgrade_Tool()
{
  myPrecision = myMinTol = Precision::Confusion();
  myMaxTol = 1; //Precision::Infinite() ?? pdn
}

//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

void ShapeUpgrade_Tool::Set(const Handle(ShapeUpgrade_Tool)& tool)
{
  myContext   = tool->myContext;
  myPrecision = tool->myPrecision;
  myMinTol    = tool->myMinTol;
  myMaxTol    = tool->myMaxTol;
}

