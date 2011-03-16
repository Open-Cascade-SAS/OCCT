// File:	PDataStd_Relation.cxx
// Created:	Wed Dec 10 11:26:08 1997
// Author:	Denis PASCAL
//		<dp@dingox.paris1.matra-dtv.fr>

#include <PDataStd_Relation.ixx>

//=======================================================================
//function : PDataStd_Relation
//purpose  : 
//=======================================================================

PDataStd_Relation::PDataStd_Relation()
{
}

//=======================================================================
//function : PDataStd_Relation
//purpose  : 
//=======================================================================

PDataStd_Relation::PDataStd_Relation(const Handle(PCollection_HExtendedString)& name)
{
  SetName(name);
}

//=======================================================================
//function : GetName
//purpose  : 
//=======================================================================

Handle(PCollection_HExtendedString) PDataStd_Relation::GetName() const
{
  return myName;
}

//=======================================================================
//function : SetName
//purpose  : 
//=======================================================================

void PDataStd_Relation::SetName(const Handle(PCollection_HExtendedString)& name)
{
  myName = name;
}


//=======================================================================
//function : GetVariables
//purpose  : 
//=======================================================================

Handle(PDF_HAttributeArray1) PDataStd_Relation::GetVariables() const
{ 
  return myVariables; 
}


//=======================================================================
//function : SetVariables
//purpose  : 
//=======================================================================

void PDataStd_Relation::SetVariables(const Handle(PDF_HAttributeArray1)& vars) 
{ 
  myVariables = vars; 
}
