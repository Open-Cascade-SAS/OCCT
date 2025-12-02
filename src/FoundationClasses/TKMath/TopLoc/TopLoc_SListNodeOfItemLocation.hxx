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

#ifndef _TopLoc_SListNodeOfItemLocation_HeaderFile
#define _TopLoc_SListNodeOfItemLocation_HeaderFile

#include <Standard.hxx>

#include <TopLoc_SListOfItemLocation.hxx>
#include <TopLoc_ItemLocation.hxx>
#include <Standard_Transient.hxx>

class TopLoc_SListNodeOfItemLocation;
DEFINE_STANDARD_HANDLE(TopLoc_SListNodeOfItemLocation, Standard_Transient)

class TopLoc_SListNodeOfItemLocation : public Standard_Transient
{

public:
  TopLoc_SListNodeOfItemLocation(const TopLoc_ItemLocation&        I,
                                 const TopLoc_SListOfItemLocation& aTail);

  TopLoc_SListOfItemLocation& Tail() const;

  TopLoc_ItemLocation& Value() const;

  DEFINE_STANDARD_RTTIEXT(TopLoc_SListNodeOfItemLocation, Standard_Transient)

protected:
private:
  TopLoc_SListOfItemLocation myTail;
  TopLoc_ItemLocation        myValue;
};

#include <TopLoc_SListNodeOfItemLocation.lxx>

#endif // _TopLoc_SListNodeOfItemLocation_HeaderFile
