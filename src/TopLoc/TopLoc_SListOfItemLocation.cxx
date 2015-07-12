// Created on: 1993-02-26
// Created by: Remi LEQUETTE
// Copyright (c) 1993-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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


#include <Standard_NoSuchObject.hxx>
#include <TopLoc_ItemLocation.hxx>
#include <TopLoc_SListNodeOfItemLocation.hxx>
#include <TopLoc_SListOfItemLocation.hxx>

//=======================================================================
//function : TopLoc_SListOfItemLocation
//purpose  : 
//=======================================================================
TopLoc_SListOfItemLocation::TopLoc_SListOfItemLocation(const TopLoc_ItemLocation& anItem,
				     const TopLoc_SListOfItemLocation& aTail) : 
       myNode(new TopLoc_SListNodeOfItemLocation(anItem,aTail))
{
  if (!myNode->Tail().IsEmpty()) {
    const gp_Trsf& aT = myNode->Tail().Value().myTrsf;
    myNode->Value().myTrsf.PreMultiply (aT);
  }
}

//=======================================================================
//function : Assign
//purpose  : 
//=======================================================================

TopLoc_SListOfItemLocation& TopLoc_SListOfItemLocation::Assign(const TopLoc_SListOfItemLocation& Other)
{
  if (this == &Other) return *this;
  Clear();
  myNode = Other.myNode;

  return *this;
}

//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

const TopLoc_ItemLocation& TopLoc_SListOfItemLocation::Value() const
{
  Standard_NoSuchObject_Raise_if(myNode.IsNull(),"TopLoc_SListOfItemLocation::Value");
  return myNode->Value();
}

//=======================================================================
//function : Tail
//purpose  : 
//=======================================================================

const TopLoc_SListOfItemLocation& TopLoc_SListOfItemLocation::Tail() const
{
  if (!myNode.IsNull()) 
    return  myNode->Tail();
  else
    return *this;
}
