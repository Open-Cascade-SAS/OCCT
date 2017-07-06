// Created on: 2008-01-21
// Created by: Galina KULIKOVA
// Copyright (c) 2008-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.


#include <Interface_FileParameter.hxx>
#include <Interface_ParamList.hxx>
#include <Standard_OutOfMemory.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_RangeError.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Interface_ParamList,Standard_Transient)

//=======================================================================
//function : Interface_ParamList
//purpose  : 
//=======================================================================
Interface_ParamList::Interface_ParamList(const Standard_Integer theIncrement) :
        myVector (theIncrement)
{
  
}

//=======================================================================
//function : SetValue
//purpose  : 
//=======================================================================

void Interface_ParamList::SetValue(const Standard_Integer theIndex,const Interface_FileParameter& theValue) 
{
  Standard_Integer ind = theIndex-1;
  myVector.SetValue(ind,theValue);
}

 
//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

const Interface_FileParameter& Interface_ParamList::Value(const Standard_Integer theIndex) const
{
  Standard_Integer ind = theIndex-1;
  return myVector.Value(ind);
}

 
//=======================================================================
//function : ChangeValue
//purpose  : 
//=======================================================================

Interface_FileParameter& Interface_ParamList::ChangeValue(const Standard_Integer theIndex) 
{
  Standard_Integer ind = theIndex-1;
  if(ind >= myVector.Length())
  {
    Interface_FileParameter aFP;
    myVector.SetValue(ind,aFP);
  }    
  return myVector.ChangeValue(ind);
}

void Interface_ParamList::Clear()
{
  myVector.Clear();
}
