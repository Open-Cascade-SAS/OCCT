// File:	PNaming_Naming.cxx
// Created:	Thu Oct 21 12:46:20 1999
// Author:	Denis PASCAL
//		<dp@dingox.paris1.matra-dtv.fr>


#include <PNaming_Naming.ixx>


//=======================================================================
//function : PNaming_Naming
//purpose  : 
//=======================================================================

PNaming_Naming::PNaming_Naming() {}


//=======================================================================
//function : SetName
//purpose  : 
//=======================================================================

void PNaming_Naming::SetName(const Handle(PNaming_Name)& Name) 
{myName = Name ;}

//=======================================================================
//function : GetName
//purpose  : 
//=======================================================================

Handle(PNaming_Name) PNaming_Naming::GetName() const
{
  return myName;
}
