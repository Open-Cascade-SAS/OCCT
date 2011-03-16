// File:	PNaming_Naming_1.cxx
// Created:	Fri Aug 15 18:21:18 2008
// Author:	Sergey ZARITCHNY <szy@opencascade.com>
// Copyright:   Open CasCade SA 2008

#include <PNaming_Naming_1.ixx>

//=======================================================================
//function : PNaming_Naming_1
//purpose  : 
//=======================================================================

PNaming_Naming_1::PNaming_Naming_1() {}


//=======================================================================
//function : SetName
//purpose  : 
//=======================================================================

void PNaming_Naming_1::SetName(const Handle(PNaming_Name_1)& Name) 
{myName = Name ;}

//=======================================================================
//function : GetName
//purpose  : 
//=======================================================================

Handle(PNaming_Name_1) PNaming_Naming_1::GetName() const
{
  return myName;
}
