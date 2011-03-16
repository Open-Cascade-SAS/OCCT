// File:        PDataStd_ByteArray.cxx
// Created:     May 29 11:40:00 2007
// Author:      Vlad Romashko
//  	    	<vladislav.romashko@opencascade.com>
// Copyright:   Open CASCADE

#include <PDataStd_ByteArray.ixx>

//=======================================================================
//function : PDataStd_ByteArray
//purpose  : 
//=======================================================================
PDataStd_ByteArray::PDataStd_ByteArray() 
{ 

}

//=======================================================================
//function : Set
//purpose  : 
//=======================================================================
void PDataStd_ByteArray::Set(const Handle(PColStd_HArray1OfInteger)& values)
{
  myValues = values;
}

//=======================================================================
//function : Get
//purpose  : 
//=======================================================================
const Handle(PColStd_HArray1OfInteger)& PDataStd_ByteArray::Get() const
{
  return myValues;
}
