// File:	ShapeProcess_UOperator.cxx
// Created:	Tue Aug 22 13:29:08 2000
// Author:	Andrey BETENEV
//		<abv@nnov.matra-dtv.fr>

#include <ShapeProcess_UOperator.ixx>

//=======================================================================
//function : ShapeProcess_UOperator
//purpose  : 
//=======================================================================

ShapeProcess_UOperator::ShapeProcess_UOperator (const ShapeProcess_OperFunc func) : myFunc(func)
{
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

Standard_Boolean ShapeProcess_UOperator::Perform (const Handle(ShapeProcess_Context)& context)
{
  return myFunc ( context );
}
