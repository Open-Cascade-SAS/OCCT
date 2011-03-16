// File:	PDataStd_Expression.cxx
// Created:	Wed Dec 10 11:26:08 1997
// Author:	Denis PASCAL
//		<dp@dingox.paris1.matra-dtv.fr>

#include <PDataStd_Expression.ixx>

//=======================================================================
//function : PDataStd_Expression
//purpose  : 
//=======================================================================

PDataStd_Expression::PDataStd_Expression()
{
}

//=======================================================================
//function : PDataStd_Expression
//purpose  : 
//=======================================================================

PDataStd_Expression::PDataStd_Expression(const Handle(PCollection_HExtendedString)& name)
{
  SetName(name);
}

//=======================================================================
//function : GetName
//purpose  : 
//=======================================================================

 Handle(PCollection_HExtendedString) PDataStd_Expression::GetName() const
{
  return myName;
}

//=======================================================================
//function : SetName
//purpose  : 
//=======================================================================

 void PDataStd_Expression::SetName(const Handle(PCollection_HExtendedString)& name) 
{
  myName = name;
}


//=======================================================================
//function : GetVariables
//purpose  : 
//=======================================================================

Handle(PDF_HAttributeArray1) PDataStd_Expression::GetVariables() const
{ 
  return myVariables; 
}


//=======================================================================
//function : SetVariables
//purpose  : 
//=======================================================================

void PDataStd_Expression::SetVariables(const Handle(PDF_HAttributeArray1)& vars) 
{ 
  myVariables = vars; 
}
