// Created on: 2000-08-15
// Created by: data exchange team
// Copyright (c) 2000-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <PXCAFDoc_Location.ixx>

//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================

PXCAFDoc_Location::PXCAFDoc_Location()
{
}

//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================

PXCAFDoc_Location::PXCAFDoc_Location(const PTopLoc_Location& Loc):
       myPLocation(Loc)
{
}

//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

 void PXCAFDoc_Location::Set(const PTopLoc_Location& Loc) 
{
  myPLocation = Loc;
}

//=======================================================================
//function : Get
//purpose  : 
//=======================================================================

 PTopLoc_Location PXCAFDoc_Location::Get() const
{
  return myPLocation;
}

