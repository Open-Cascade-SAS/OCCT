// File:	PDataStd_UAttribute.cxx
// Created:	Tue Jun  15 13:24:14 1999
// Author:	Sergey RUIN

#include <PDataStd_UAttribute.ixx>
#include <PCollection_HExtendedString.hxx>

//=======================================================================
//function : PDataStd_UAttribute
//purpose  : 
//=======================================================================

PDataStd_UAttribute::PDataStd_UAttribute () { }


//=======================================================================
//function : SetID
//purpose  : 
//=======================================================================

void PDataStd_UAttribute::SetID( const Handle(PCollection_HExtendedString)&  guid)
{
  myID = guid;
}

//=======================================================================
//function : GetLocalID
//purpose  : 
//=======================================================================

Handle(PCollection_HExtendedString) PDataStd_UAttribute::GetID( void ) const
{
  return myID;
}
