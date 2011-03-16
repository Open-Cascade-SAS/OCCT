// File:	PDataStd_Variable.cxx
// Created:	Wed Dec 10 11:26:08 1997
// Author:	Denis PASCAL
//		<dp@dingox.paris1.matra-dtv.fr>

#include <PDataStd_Variable.ixx>

//=======================================================================
//function : PDataStd_Variable
//purpose  : 
//=======================================================================

PDataStd_Variable::PDataStd_Variable()
{
}

//=======================================================================
//function : PDataStd_Variable
//purpose  : 
//=======================================================================

PDataStd_Variable::PDataStd_Variable(const Standard_Boolean constant)
{
  Constant(constant);
}

//=======================================================================
//function : Constant
//purpose  : 
//=======================================================================

void PDataStd_Variable::Constant(const Standard_Boolean status) 
{
  isConstant = status;
}
//=======================================================================
//function : Constant
//purpose  : 
//=======================================================================
Standard_Boolean PDataStd_Variable::Constant() const
{
  return isConstant;
}
//=======================================================================
//function : Unit
//purpose  : 
//=======================================================================

void PDataStd_Variable::Unit(const Handle(PCollection_HAsciiString)& unit) 
{
  myUnit = unit;
}
//=======================================================================
//function : Unit
//purpose  : 
//=======================================================================
Handle(PCollection_HAsciiString) PDataStd_Variable::Unit() const
{
  return myUnit;
}
